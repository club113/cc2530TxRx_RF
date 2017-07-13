import serial
import time
import threading
import struct
det_msg = (b'\x5A\xA5\x10\x01\x06\x00\x00\x00\x13\x01\x01\x01\xE3\xB5\x0A\x00\x4B\x12\x00\x01\x01\x01\x00\x00\x00\x00\x00\x40\xA5\x5A')
def MyTxThread():
	x = 1
	circle = 0
	while x < 50:
		x = 1 + x
		print('%d'% x)
		time.sleep(2)
		 
#display hex string 	
def HexShow(i_string):
	hex_string = ''
	hLen = len(i_string)
	for i in range(hLen):
		hvol = i_string[i]
		hhex = '%02X' % (hvol)
		hex_string += hhex + ' '
#	print('ReceiveBytes: %i_string' % (hex_string))
	print('Hex:',hex_string,'  total:%d\n'%hLen)
	
	
#Comnumb = 'com19'
Comnumb=input('输入串口号(如com4):')
s = serial.Serial(Comnumb,115200)
string = (b'')
s.setTimeout(1)
#thrd = threading.Thread(target=MyTxThread,name='koing2010')
#thrd.start()
for i in range(10000):
#	print('第',i,'次读取')
	while s.inWaiting() == 0:#wait ack
		pass
	time.sleep(0.003)#delay 10ms
	numb = s.inWaiting()#read the numb of bytes received
	string = s.read(numb)
	print("RSSI  -%d"%(256-string[numb-2]))
s.close()
#thrd.join()
