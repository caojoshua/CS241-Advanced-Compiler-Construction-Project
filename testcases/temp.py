import requests

zero="00"
for i in range(1,32):
	print(i)
	if i == 10:
		zero = "0"
	fname = "test" + zero + str(i) + ".txt"
	r = requests.get("https://www.michaelfranz.com/CS241/testprogs/" + fname)
	txt = r.text
	f = open(fname,"w")
	for line in txt:
		f.write(line)
	f.close()
	
# import requests
# import urllib.request

# zero="00"
# for i in range(1,32):
	# print(i)
	# if i == 10:
		# zero = "0"
	# fname = "test" + zero + str(i) + ".txt"
	# r = urllib.request.urlopen("https://www.michaelfranz.com/CS241/testprogs/" + fname)
	# f = open(fname,"w")
	# for line in r:
		# print(line)
		# line = line.decode("utf-8")
		# line = line.replace("\n","")
		# print(bytearray(line,"utf-8"))
		# f.write(line)
	# f.close()