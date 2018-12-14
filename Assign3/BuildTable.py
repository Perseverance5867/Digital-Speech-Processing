datar = open("Big5-ZhuYin.map", "r", encoding = 'big5hkscs').read()
dataw = open("ZhuYin-Big5.map", "w", encoding = 'big5hkscs')

dictionary = {}
for line in datar.split('\n'):
	if len(line) == 0: continue


	word, sylb = line.split(' ')
	dictionary[word] = word
	
	char = sylb.split('/')
	for k in char:
		if k[0] in dictionary:		dictionary[k[0]].append(word)
		if k[0] not in dictionary:	dictionary[k[0]] = list(word)

for key in dictionary:
	value = ' '.join(dictionary[key])
	line = key + "\t" + value + "\n"
	dataw.write(line)
	# print (line)

