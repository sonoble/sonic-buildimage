from unittest import TestCase
import subprocess
import os

class TestCfgGen(TestCase):

    def setUp(self):
        self.test_dir = os.path.dirname(os.path.realpath(__file__))
        self.script_file = os.path.join(self.test_dir, '..', 'sonic-cfggen')
        self.sample_graph = os.path.join(self.test_dir, 'sample_graph.xml')
        self.sample_graph_t0 = os.path.join(self.test_dir, 't0-sample-graph.xml')
        self.sample_graph_simple = os.path.join(self.test_dir, 'simple-sample-graph.xml')
        self.sample_graph_metadata = os.path.join(self.test_dir, 'simple-sample-graph-metadata.xml')
        self.sample_graph_pc_test = os.path.join(self.test_dir, 'pc-test-graph.xml')
        self.sample_graph_bgp_speaker = os.path.join(self.test_dir, 't0-sample-bgp-speaker.xml')
        self.sample_device_desc = os.path.join(self.test_dir, 'device.xml')
        self.port_config = os.path.join(self.test_dir, 't0-sample-port-config.ini')

    def run_script(self, argument, check_stderr=False):
        print '\n    Running sonic-cfggen ' + argument
        if check_stderr:
            output = subprocess.check_output(self.script_file + ' ' + argument, stderr=subprocess.STDOUT, shell=True)
        else:
            output = subprocess.check_output(self.script_file + ' ' + argument, shell=True)

        linecount = output.strip().count('\n')
        if linecount <= 0:
            print '    Output: ' + output.strip()
        else:
            print '    Output: ({0} lines, {1} bytes)'.format(linecount + 1, len(output))
        return output

    def test_dummy_run(self):
        argument = ''
        output = self.run_script(argument)
        self.assertEqual(output, '')

    def test_device_desc(self):
        argument = '-v "DEVICE_METADATA[\'localhost\'][\'hwsku\']" -M "' + self.sample_device_desc + '"'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), 'ACS-MSN2700')

    def test_device_desc_mgmt_ip(self):
        argument = '-v "MGMT_INTERFACE.keys()[0]" -M "' + self.sample_device_desc + '"'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "('eth0', '10.0.1.5/28')")

    def test_minigraph_sku(self):
        argument = '-v "DEVICE_METADATA[\'localhost\'][\'hwsku\']" -m "' + self.sample_graph + '"'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), 'Force10-Z9100')

    def test_print_data(self):
        argument = '-m "' + self.sample_graph + '" --print-data'
        output = self.run_script(argument)
        self.assertTrue(len(output.strip()) > 0)

    def test_jinja_expression(self):
        argument = '-m "' + self.sample_graph + '" -v "DEVICE_METADATA[\'localhost\'][\'type\']"'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), 'LeafRouter')

    def test_additional_json_data(self):
        argument = '-a \'{"key1":"value1"}\' -v key1'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), 'value1')

    def test_read_yaml(self):
        argument = '-v yml_item -y ' + os.path.join(self.test_dir, 'test.yml')
        output = self.run_script(argument)
        self.assertEqual(output.strip(), '[\'value1\', \'value2\']')

    def test_render_template(self):
        argument = '-y ' + os.path.join(self.test_dir, 'test.yml') + ' -t ' + os.path.join(self.test_dir, 'test.j2')
        output = self.run_script(argument)
        self.assertEqual(output.strip(), 'value1\nvalue2')

    def test_minigraph_acl(self):
        argument = '-m "' + self.sample_graph_t0 + '" -p "' + self.port_config + '" -v ACL_TABLE'
        output = self.run_script(argument, True)
        self.assertEqual(output.strip(), "Warning: Ignoring Control Plane ACL NTP_ACL without type\n"
                                         "Warning: ignore interface 'fortyGigE0/2' as it is not in the port_config.ini\n"
                                         "Warning: ignore interface 'fortyGigE0/2' in DEVICE_NEIGHBOR as it is not in the port_config.ini\n"
                                         "{'SSH_ACL': {'services': ['SSH'], 'type': 'CTRLPLANE', 'policy_desc': 'SSH_ACL'},"
                                         " 'SNMP_ACL': {'services': ['SNMP'], 'type': 'CTRLPLANE', 'policy_desc': 'SNMP_ACL'},"
                                         " 'DATAACL': {'type': 'L3', 'policy_desc': 'DATAACL', 'ports': ['PortChannel01', 'PortChannel02', 'PortChannel03', 'PortChannel04']},"
                                         " 'NTP_ACL': {'services': ['NTP'], 'type': 'CTRLPLANE', 'policy_desc': 'NTP_ACL'},"
                                         " 'ROUTER_PROTECT': {'services': ['SSH', 'SNMP'], 'type': 'CTRLPLANE', 'policy_desc': 'ROUTER_PROTECT'}}")

    def test_minigraph_everflow(self):
        argument = '-m "' + self.sample_graph_t0 + '" -p "' + self.port_config + '" -v MIRROR_SESSION'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "{'everflow0': {'src_ip': '10.1.0.32', 'dst_ip': '2.2.2.2'}}")

    def test_minigraph_interfaces(self):
        argument = '-m "' + self.sample_graph_simple + '" -p "' + self.port_config + '" -v \'INTERFACE.keys()\''
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "[('Ethernet0', '10.0.0.58/31'), ('Ethernet0', 'FC00::75/126')]")

    def test_minigraph_vlans(self):
        argument = '-m "' + self.sample_graph_simple + '" -p "' + self.port_config + '" -v VLAN'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "{'Vlan1000': {'alias': 'ab1', 'dhcp_servers': ['192.0.0.1', '192.0.0.2'], 'vlanid': '1000'}}")

    def test_minigraph_vlan_members(self):
        argument = '-m "' + self.sample_graph_simple + '" -p "' + self.port_config + '" -v VLAN_MEMBER'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "{'Vlan1000|Ethernet8': {'tagging_mode': 'untagged'}}")

    def test_minigraph_vlan_interfaces(self):
        argument = '-m "' + self.sample_graph_simple + '" -p "' + self.port_config + '" -v "VLAN_INTERFACE.keys()"'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "[('Vlan1000', '192.168.0.1/27')]")

    def test_minigraph_portchannels(self):
        argument = '-m "' + self.sample_graph_simple + '" -p "' + self.port_config + '" -v PORTCHANNEL'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "{'PortChannel01': {'members': ['Ethernet4']}}")

    def test_minigraph_portchannels_more_member(self):
        argument = '-m "' + self.sample_graph_pc_test + '" -p "' + self.port_config + '" -v PORTCHANNEL'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "{'PortChannel01': {'members': ['Ethernet112', 'Ethernet116', 'Ethernet120', 'Ethernet124']}}")

    def test_minigraph_portchannel_interfaces(self):
        argument = '-m "' + self.sample_graph_simple + '" -p "' + self.port_config + '" -v "PORTCHANNEL_INTERFACE.keys()"'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "[('PortChannel01', 'FC00::71/126'), ('PortChannel01', '10.0.0.56/31')]")

    def test_minigraph_neighbors(self):
        argument = '-m "' + self.sample_graph_t0 + '" -p "' + self.port_config + '" -v "DEVICE_NEIGHBOR[\'Ethernet124\']"'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "{'name': 'ARISTA04T1', 'port': 'Ethernet1/1'}")

    def test_minigraph_extra_neighbors(self):
        argument = '-m "' + self.sample_graph_t0 + '" -p "' + self.port_config + '" -v DEVICE_NEIGHBOR'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), \
                "{'Ethernet116': {'name': 'ARISTA02T1', 'port': 'Ethernet1/1'}, "
                "'Ethernet124': {'name': 'ARISTA04T1', 'port': 'Ethernet1/1'}, "
                "'Ethernet112': {'name': 'ARISTA01T1', 'port': 'Ethernet1/1'}, "
                "'Ethernet120': {'name': 'ARISTA03T1', 'port': 'Ethernet1/1'}}")

    def test_minigraph_bgp(self):
        argument = '-m "' + self.sample_graph_bgp_speaker + '" -p "' + self.port_config + '" -v "BGP_NEIGHBOR[\'10.0.0.59\']"'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "{'rrclient': 0, 'name': 'ARISTA02T1', 'local_addr': '10.0.0.58', 'nhopself': 0, 'holdtime': '180', 'asn': '64600', 'keepalive': '60'}")

    def test_minigraph_peers_with_range(self):
        argument = '-m "' + self.sample_graph_bgp_speaker + '" -p "' + self.port_config + '" -v BGP_PEER_RANGE.values\(\)'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "[{'name': 'BGPSLBPassive', 'ip_range': ['10.10.10.10/26', '100.100.100.100/26']}]")

    def test_minigraph_deployment_id(self):
        argument = '-m "' + self.sample_graph_bgp_speaker + '" -p "' + self.port_config + '" -v "DEVICE_METADATA[\'localhost\'][\'deployment_id\']"'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "1")

    def test_minigraph_ethernet_interfaces(self):
        argument = '-m "' + self.sample_graph_simple + '" -p "' + self.port_config + '" -v "PORT[\'Ethernet8\']"'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "{'alias': 'fortyGigE0/8', 'lanes': '37,38,39,40', 'description': 'Interface description', 'speed': '1000'}")
        argument = '-m "' + self.sample_graph_simple + '" -p "' + self.port_config + '" -v "PORT[\'Ethernet12\']"'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "{'alias': 'fortyGigE0/12', 'lanes': '33,34,35,36', 'fec': 'rs', 'speed': '100000', 'description': 'Interface description'}")

    def test_minigraph_extra_ethernet_interfaces(self):
        argument = '-m "' + self.sample_graph_simple + '" -p "' + self.port_config + '" -v "PORT"'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), \
                "{'Ethernet8': {'alias': 'fortyGigE0/8', 'lanes': '37,38,39,40', 'description': 'Interface description', 'speed': '1000'}, "
                "'Ethernet0': {'alias': 'fortyGigE0/0', 'lanes': '29,30,31,32', 'speed': '10000'}, "
                "'Ethernet4': {'alias': 'fortyGigE0/4', 'lanes': '25,26,27,28', 'speed': '25000'}, "
                "'Ethernet108': {'alias': 'fortyGigE0/108', 'lanes': '81,82,83,84'}, "
                "'Ethernet100': {'alias': 'fortyGigE0/100', 'lanes': '125,126,127,128'}, "
                "'Ethernet104': {'alias': 'fortyGigE0/104', 'lanes': '85,86,87,88'}, "
                "'Ethernet68': {'alias': 'fortyGigE0/68', 'lanes': '69,70,71,72'}, "
                "'Ethernet96': {'alias': 'fortyGigE0/96', 'lanes': '121,122,123,124'}, "
                "'Ethernet124': {'alias': 'fortyGigE0/124', 'lanes': '101,102,103,104'}, "
                "'Ethernet92': {'alias': 'fortyGigE0/92', 'lanes': '113,114,115,116'}, "
                "'Ethernet120': {'alias': 'fortyGigE0/120', 'lanes': '97,98,99,100'}, "
                "'Ethernet52': {'alias': 'fortyGigE0/52', 'lanes': '53,54,55,56'}, "
                "'Ethernet56': {'alias': 'fortyGigE0/56', 'lanes': '61,62,63,64'}, "
                "'Ethernet76': {'alias': 'fortyGigE0/76', 'lanes': '73,74,75,76'}, "
                "'Ethernet72': {'alias': 'fortyGigE0/72', 'lanes': '77,78,79,80'}, "
                "'Ethernet64': {'alias': 'fortyGigE0/64', 'lanes': '65,66,67,68'}, "
                "'Ethernet32': {'alias': 'fortyGigE0/32', 'lanes': '9,10,11,12'}, "
                "'Ethernet16': {'alias': 'fortyGigE0/16', 'lanes': '41,42,43,44'}, "
                "'Ethernet36': {'alias': 'fortyGigE0/36', 'lanes': '13,14,15,16'}, "
                "'Ethernet12': {'alias': 'fortyGigE0/12', 'lanes': '33,34,35,36', 'fec': 'rs', 'speed': '100000', 'description': 'Interface description'}, "
                "'Ethernet88': {'alias': 'fortyGigE0/88', 'lanes': '117,118,119,120'}, "
                "'Ethernet116': {'alias': 'fortyGigE0/116', 'lanes': '93,94,95,96'}, "
                "'Ethernet80': {'alias': 'fortyGigE0/80', 'lanes': '105,106,107,108'}, "
                "'Ethernet112': {'alias': 'fortyGigE0/112', 'lanes': '89,90,91,92'}, "
                "'Ethernet84': {'alias': 'fortyGigE0/84', 'lanes': '109,110,111,112'}, "
                "'Ethernet48': {'alias': 'fortyGigE0/48', 'lanes': '49,50,51,52'}, "
                "'Ethernet44': {'alias': 'fortyGigE0/44', 'lanes': '17,18,19,20'}, "
                "'Ethernet40': {'alias': 'fortyGigE0/40', 'lanes': '21,22,23,24'}, "
                "'Ethernet28': {'alias': 'fortyGigE0/28', 'lanes': '1,2,3,4'}, "
                "'Ethernet60': {'alias': 'fortyGigE0/60', 'lanes': '57,58,59,60'}, "
                "'Ethernet20': {'alias': 'fortyGigE0/20', 'lanes': '45,46,47,48'}, "
                "'Ethernet24': {'alias': 'fortyGigE0/24', 'lanes': '5,6,7,8'}}")

    def test_metadata_everflow(self):
        argument = '-m "' + self.sample_graph_metadata + '" -p "' + self.port_config + '" -v "MIRROR_SESSION"'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "{'everflow0': {'src_ip': '10.1.0.32', 'dst_ip': '10.0.100.1'}}")

    def test_metadata_tacacs(self):
        argument = '-m "' + self.sample_graph_metadata + '" -p "' + self.port_config + '" -v "TACPLUS_SERVER"'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "{'10.0.10.7': {'priority': '1', 'tcp_port': '49'}, '10.0.10.8': {'priority': '1', 'tcp_port': '49'}}")

    def test_metadata_ntp(self):
        argument = '-m "' + self.sample_graph_metadata + '" -p "' + self.port_config + '" -v "NTP_SERVER"'
        output = self.run_script(argument)
        self.assertEqual(output.strip(), "{'10.0.10.1': {}, '10.0.10.2': {}}")

