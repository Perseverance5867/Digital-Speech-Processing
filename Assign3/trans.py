import sys
with open(sys.argv[1], "r", encoding='big5hkscs') as f:
    data = f.read()
    print (data)
