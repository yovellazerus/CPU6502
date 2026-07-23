
#include "comman.h"

/*====== RAM =========================================================================*/

static char arena[3*4096];
static char* top = arena;
static char* end = arena + sizeof(arena);

static void* sbrk(uint16_t bytes){
    char* res = top;
    if(top + bytes >= end){
        return (void*)-1;
    }
    top += bytes;
    return res;
}

/*====== malloc =========================================================================*/

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef struct Header {
    struct Header* next;
    uint16_t size;
} Header;

static Header  base;
static Header* freep;

void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  for (p = freep; !(bp > p && bp < p->next); p = p->next)
    if (p >= p->next && (bp > p || bp < p->next))
      break;
  if (bp + bp->size == p->next) {
    bp->size += p->next->size;
    bp->next = p->next->next;
  } else
    bp->next = p->next;
  if (p + p->size == bp) {
    p->size += bp->size;
    p->next = bp->next;
  } else
    p->next = bp;
  freep = p;
}

static Header *
morecore(uint16_t nu)
{
  char* p;
  Header* hp;

  p = sbrk(nu * sizeof(Header));
  if (p == (char*)-1)
    return 0;
  hp = (Header *)p;
  hp->size = nu;
  free((void *)(hp + 1));
  return freep;
}

void *
malloc(uint16_t nbytes)
{
  Header*p, *prevp;
  uint16_t nunits;

  nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
  if ((prevp = freep) == 0) {
    base.next = freep = prevp = &base;
    base.size = 0;
  }
  for (p = prevp->next;; prevp = p, p = p->next) {
    if (p->size >= nunits) {
      if (p->size == nunits)
        prevp->next = p->next;
      else {
        p->size -= nunits;
        p += p->size;
        p->size = nunits;
      }
      freep = prevp;
      return (void *)(p + 1);
    }
    if (p == freep)
      if ((p = morecore(nunits)) == 0)
        return 0;
  }
}

/*========== Tree ===================================================================*/

typedef struct Node {
    int key;
    struct Node* l;
    struct Node* r;
} Node;

Node* Node_create(int key){
    Node* res;
    res = malloc(sizeof(*res));
    if(!res) return NULL;
    res->key = key;
    res->l = NULL;
    res->r = NULL;
    return res;
}
void Node_destroy(Node* n){
    if(!n) return;
    Node_destroy(n->l);
    Node_destroy(n->r);
    free(n);
}
void Node_dump(const Node* n){
    if(!n) return;
    if(n->l) printk("\t%d -> %d\n", n->key, n->l->key);
    if(n->r) printk("\t%d -> %d\n", n->key, n->r->key);
    Node_dump(n->l);
    Node_dump(n->r);
}

typedef struct {
    Node*  root;
    uint16_t size;
} Tree;

Tree* Tree_create(void){
    Tree* res;
    res = malloc(sizeof(*res));
    if(!res) return NULL;
    res->root = NULL;
    res->size = 0;
    return res;
}
void Tree_destroy(Tree* t){
    if(!t) return;
    Node_destroy(t->root);
    free(t);
}
void Tree_dump(const Tree* t){
    if(!t) return;
    printk("digraph Tree {\n");
    Node_dump(t->root);
    printk("}\n");
}
void Tree_insert(Tree* t, int key){
    Node* new_node;
    Node* curr;
    Node* prev;
    new_node = Node_create(key);
    if(!new_node) return;
    if(!t->root){
        t->root = new_node;
        t->size++;
        return;
    }
    curr = t->root;
    prev = NULL;
    while(curr){
        prev = curr;
        if(curr->key < key){
            curr = curr->r;
        }
        else if(curr->key > key){
            curr = curr->l;
        }
        else{
            Node_destroy(new_node);
            return;
        }
    }
    if(prev->key < key){
        prev->r = new_node;
    }
    else if(prev->key > key){
        prev->l = new_node;
    }
    t->size++;
}

static uint32_t seed = 1;

void srand(uint32_t s)
{
    seed = s;
}

uint32_t rand(void)
{
    seed = seed * 1664525u + 1013904223u;
    return seed;
}

int Tree_test(void){
    int i;
    Tree* tree = Tree_create();
    if(!tree){
        printk("Tree_create");
        return 1;
    }
    srand(42);
    for(i = 0; i < 100; i++){
        Tree_insert(tree,  rand() % 100);
    }
    Tree_dump(tree);
    Tree_destroy(tree);
    
    return 0;
}

/*===========================================================================================*/

void hardware_init(void){

    // mapout the ROM, and install the irq/nmi kernel handlers
    MMIO8(MMU_ROM_ENABLE) = MMU_ROM_ENABLE_FALSE;
    MMIO16(0xfffa) = (uint16_t)nmi_handler;
    MMIO16(0xfffe) = (uint16_t)irq_handler;

    // ...
}

void proc_init(void){

}

void fs_init(void){

}
    
void run_init_process(void){

}

void start_timer(void){
    
}

Context ctx = {
    0xff,
    0x00,
    0x0200,
    0x00,
    0x00,
    0x00
};

// testing that writing to MMU is not possible from user space
uint8_t code[] = {
    0xa9, 0x42,          // lda #$42
    0x8d, 0x26, 0xfe,    // sta $SEG15
    0x4c, 0x05, 0x02,    // jmp $0205
};

void main(void) {

    Proc* p = NULL;
    int i = 0;

    hardware_init();

    printk("******* kernel v%d ********\n", 1);

    kalloc_init();
    proc_init();
    fs_init();

    run_init_process();

    start_timer();
    asm("cli");

    // user space test
    p = palloc();
    if(!p){
        panic("palloc");
    }
    for(i = 0; i < 16; i++){
        p->page_table[i] = kalloc();
        if(p->page_table[i] == FRAME_UNUSED){
            panic("no more frames");
        }
    }
    if(copy_to_user(code, 0x0200, sizeof(code), p->page_table) < 0){
        panic("copy_to_user");
    }
    copy_to_life_raft(&ctx, p->page_table, kernel_page_table);
    return_from_trap();

    // no return
    scheduler();
}
