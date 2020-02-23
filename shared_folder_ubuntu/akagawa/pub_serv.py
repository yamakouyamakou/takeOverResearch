import rospy
from std_msgs.msg import Int16
import time
def talker():
	pub = rospy.Publisher('servo',Int16,queue_size = 10)
	rospy.init_node('talker')
	r = rospy.Rate(0.5)
	msg = 0
	while not rospy.is_shutdown():
		with open("flag.txt",'r') as f:
			flag = int(f.read())
			print(flag)
		if flag == 1:
			msg = 10
			with open("flag.txt",'w')as n:
				n.write('0')
		else:
			msg = 0
	
			
		rospy.loginfo(msg)
		pub.publish(msg)
		r.sleep()

if __name__=='__main__':
	try:
		talker()
	except rospy.ROSInterruptException: pass 
