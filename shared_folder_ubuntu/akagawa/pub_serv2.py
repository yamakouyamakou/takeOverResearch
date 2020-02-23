import rospy
from std_msgs.msg import Int16
import time
def talker():
	
        pub = rospy.Publisher('servo',Int16,queue_size = 10)
	rospy.init_node('talker')
	r = rospy.Rate(0.5)#[Hz]
	msg = 0
	goal=70#vertical angle = 42.5
	camCenter=goal/2
	addDeg=7
		
	pub.publish(msg)
	#input('wait')
	
	while not rospy.is_shutdown():
		print(  "msg = "+ str(msg) )
		print(  "goal = "+ str(goal) )

		with open("switch.txt",'r') as ff:
                                 flag = int(ff.read())
                                 print("switch =  "+ str(flag))

		if msg < goal:
			with open("switch.txt",'r') as f:
				flag = int(f.read())
				print("flag = "+ str(flag) )
			if flag == 1:
				with open("panDeg.txt",'w')as b:
                                         b.write(  str(msg-camCenter) )
                                with open("panDeg.txt",'r')as c:
                                         pan=int( c.read() )
                                         print( "pandeg = " +  str(pan) )

				#if msg>camCenter:
				#	msg+=-3
					
				pub.publish(msg)
                                time.sleep(3.0)
				with open("switch.txt",'w')as n:
                                        n.write('0')
				
				#if msg>camCenter:
				#	msg+=3
				
				msg += addDeg

				#with open("panDeg.txt",'w')as b:
				#	b.write(  str(msg-camCenter) )
				#with open("panDeg.txt",'r')as c:
				#	pan=int( c.read() )
				#	print( "pandeg = " +  str(pan) )
				#pub.publish(msg)
				#time.sleep(1.5)
				#with open("switch.txt",'w')as n:
                                #        n.write('0')

		
		elif msg>=goal:
			msg=0
			pub.publish(msg)
			time.sleep(2)
			
			with open("panDeg.txt",'w')as b:
				b.write( str(msg-camCenter) )
			with open("panDeg.txt",'r')as c:
				pan=int( c.read() )
				print( "pandeg = " +  str(pan) )
			
			with open("tiltDeg.txt",'r')as e:
				nowTilt=float( e.read() )
				print( "nowTilt = " + str(nowTilt) )

			tilt=input('wait for input of absolute tilt deg')
			print(tilt)
			with open("tiltDeg.txt",'w')as d:
				d.write( str(tilt) )
                        with open("switch.txt",'w')as n:
                                n.write('0')


		rospy.loginfo(msg)
		#pub.publish(msg)
		r.sleep()

if __name__=='__main__':
	initTilt=input( 'input init tilt'  )
	with open("tiltDeg.txt",'w')as f:
		f.write( str(initTilt) )

	with open("switch.txt",'w')as f1:
		f1.write( '1' )

	try:
		talker()
	except rospy.ROSInterruptException: pass 


