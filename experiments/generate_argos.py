import argparse

nGroups = [3, 4, 5, 10] # Number of groups

def main():
  parser = argparse.ArgumentParser(description='Run trials of a test.')
  parser.add_argument('argos_file', help='Relative path to base argos file')
  args = parser.parse_args()

  top = ""
  bot = ""

  with open(args.argos_file) as fp:

    tmp = fp.readline()
    while tmp.find("<loop_functions") == -1:
      top += tmp
      tmp = fp.readline()
    top += tmp

    # Assuming loop functions is two lines
    tmp = fp.readline()
    top += tmp

    while True:
      tmp = fp.readline()
      if not tmp: break
      bot += tmp

  for nRobots in [12, 16, 20, 40]:
  	for dist in [0.03, 0.05, 0.1, 0.2]:
	  	fp = open("distributions/%d_uniform_%.2f_density.argos" % (nRobots, dist), 'w')
	  	fp.write(top)

	  	fp.write('    <cluster center="2.5,2.5" robot_num="%d" robot_density="%.2f" />\n' % (nRobots, dist))

	  	fp.write(bot)
	  	fp.close()

  exit()

  for n in nGroups:
    fp = open("distributions/%d_in_lines.argos" % n, 'w')
    fp.write(top)

    fp.write('    <line center="3.25,1.75" robot_num="%d" robot_distance="0.3" />\n' % n)
    fp.write('    <line center="2.75,2.25" robot_num="%d" robot_distance="0.3" />\n' % n)
    fp.write('    <line center="2.25,2.75" robot_num="%d" robot_distance="0.3" />\n' % n)
    fp.write('    <line center="1.75,3.25" robot_num="%d" robot_distance="0.3" />\n' % n)

    fp.write(bot)
    fp.close()

    fp = open("distributions/%d_in_clusters.argos" % n, 'w')
    fp.write(top)

    fp.write('    <cluster center="1,2.5" robot_num="%d" robot_density="0.2" />\n' % n)
    fp.write('    <cluster center="2.5,1" robot_num="%d" robot_density="0.2" />\n' % n)
    fp.write('    <cluster center="4,2.5" robot_num="%d" robot_density="0.2" />\n' % n)
    fp.write('    <cluster center="2.5,4" robot_num="%d" robot_density="0.2" />\n' % n)

    fp.write(bot)
    fp.close()

if __name__=='__main__':
  main()
