
from Opertion import *
import Assembler_directives as ad
from Assembler_directives import ASSEMBLER_DIR_TABLE

LABELS_TABLE = {}

ALIASS_TABLE = {}

class Alias():
  def __init__(self, cmd):
    pass

class Error(BaseException):
  def __init__(self, msg):
    m_msg = msg
  def what():
    return m_msg

def label_def(cmd, pos):
  lable = cmd[0].strip(':')
  if lable in LABELS_TABLE:
    raise Error("Redefinition of label '{}' ".format(label))  
  LABELS_TABLE[lable] = pos

def alias_def(cmd):
  if cmd[0] in ALIASS_TABLE:
    raise Error("Redefinition of alias '{}' ".format(cmd[0]))
  ALIASS_TABLE[cmd[0]] = Alias(cmd)

def load_program_from_file(file_path):
  program = []
  with open(file_path, "r") as input_file:
    for line in input_file:
      cmd = line.split(' ')
      cmd[-1] = cmd[-1].strip('\n')
      to_append = False
      for token in cmd:
        if len(token) != 0:
          to_append = True
      if to_append:
        program.append(cmd)
  return program

def dump_program(program):
  for cmd in program:
    print(cmd)
    

def assembler_dir(cmd):
  if cmd[0] not in ASSEMBLER_DIR_TABLE:
    raise Error("Unknon assembler directive '{}' in program".format(cmd[0])) 
  _dir = ASSEMBLER_DIR_TABLE[cmd[0]]
  return _dir

def parse_cmd(cmd):
  if len(cmd) < 3 or len(cmd) > 5:
    raise Error("Bad format error in commend '{}'".format(cmd))
  if len(cmd[0]) != 0 or len(cmd[1]) != 0 or len(cmd[2]) == 0:
    raise Error("Bad indentetion error in commend '{}'".format(cmd))
  mmnic = cmd[2]
  if mmnic not in OPERTIONS_TABLE:
    raise Error("Unknon mmnic name '{}' in program".format(mmnic))
  OPERTIONS_TABLE[mmnic] = Opertion(cmd)

def parse_program(program):
  pos = 0
  for cmd in program:
    token = cmd[0]
    if token == "":
      parse_cmd(cmd)
    elif token[0] == '.':
      assembler_dir(cmd)()
    elif token[-1] == ':':
      label_def(cmd, pos)
    elif token[0].isalpha():
      alias_def(cmd)
  
    else:
      raise Error("Unknon token '{}' in program".format(token))
    pos += 1

if __name__ == "main":
    program = load_program_from_file(".\basic_test.s")
    dump_program(program)
    parse_program(program)

    print("")
    print(LABELS_TABLE)
    print("")
    print(ALIASS_TABLE)



