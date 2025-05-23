# * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
# * contributor license agreements.  See the NOTICE file distributed with
# * this work for additional information regarding copyright ownership.
# * The OpenAirInterface Software Alliance licenses this file to You under
# * the OAI Public License, Version 1.1  (the "License"); you may not use this file
# * except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *      http://www.openairinterface.org/?page_id=698
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# *-------------------------------------------------------------------------------
# * For more information about the OpenAirInterface (OAI) Software Alliance:
# *      contact@openairinterface.org
# */
#---------------------------------------------------------------------
#
#   Required Python Version
#     Python 3.x
#
#---------------------------------------------------------------------

#to use isfile
import logging
#time.sleep
import time
import re
import yaml

import cls_cmd

class Module_UE:

	def __init__(self, module_name, node=None, filename="ci_infra.yaml"):
		with open(filename, 'r') as f:
			all_ues = yaml.load(f, Loader=yaml.FullLoader)
			m = all_ues.get(module_name)
			if m is None:
				raise Exception(f'no such module name "{module_name}" in "{filename}"')
			self.module_name = module_name
			self.host = m['Host'] if m['Host'] != "%%current_host%%" else None
			if node is None and self.host is None:
				raise Exception(f'node not provided when needed')
			elif node is not None and self.host is None:
				self.host = node
			self.cmd_dict = {
				"attach": m.get('AttachScript'),
				"detach": m.get('DetachScript'),
				"initialize": m.get('InitScript'),
				"terminate": m.get('TermScript'),
				"getNetwork": m.get('NetworkScript'),
				"check": m.get('CheckStatusScript'),
				"dataEnable": m.get('DataEnableScript'),
				"dataDisable": m.get('DataDisableScript'),
			}
			self.interface = m.get('IF')
			self.MTU = m.get('MTU')
			self.trace = m.get('trace') == True
			self.logStore = m.get('LogStore')
			self.cmd_prefix = m.get('CmdPrefix')
			self.runIperf3Server = m.get('RunIperf3Server', True)
			self.namespace = m.get('Namespace')
			self.cnPath = m.get('CNPath')
			logging.info(f'initialized {self.module_name}@{self.host} from {filename}')

	def __str__(self):
		return f"{self.module_name}@{self.host} [IP: {self.getIP()}]"

	def __repr__(self):
		return self.__str__()

	def _command(self, cmd, silent = False):
		if cmd is None:
			raise Exception("no command provided")
		if self.host == "" or self.host == "localhost":
			c = cls_cmd.LocalCmd()
		else:
			c = cls_cmd.RemoteCmd(self.host)
		response = c.run(cmd, silent=silent)
		c.close()
		return response

#-----------------$
#PUBLIC Methods$
#-----------------$

	def initialize(self):
		if self.trace:
			raise Exception("UE tracing not implemented yet")
			self._enableTrace()
		# we first terminate to make sure the UE has been stopped
		if self.cmd_dict["detach"]:
			self._command(self.cmd_dict["detach"], silent=True)
		self._command(self.cmd_dict["terminate"], silent=True)
		ret = self._command(self.cmd_dict["initialize"])
		logging.info(f'For command: {ret.args} | return output: {ret.stdout} | Code: {ret.returncode}')
		# Here each UE returns differently for the successful initialization, requires check based on UE
		return ret.returncode == 0


	def terminate(self):
		self._command(self.cmd_dict["terminate"])
		if self.trace:
			raise Exception("UE tracing not implemented yet")
			self._disableTrace()
			return self._logCollect()
		return None

	def attach(self, attach_tries = 4, attach_timeout = 60):
		ip = None
		while attach_tries > 0:
			self._command(self.cmd_dict["attach"])
			timeout = attach_timeout
			logging.debug("Waiting for IP address to be assigned")
			while timeout > 0 and not ip:
				time.sleep(5)
				timeout -= 5
				ip = self.getIP()
			if ip:
				break
			logging.warning(f"UE did not receive IP address after {attach_timeout} s, detaching")
			attach_tries -= 1
			self._command(self.cmd_dict["detach"])
			time.sleep(5)
		if ip:
			logging.debug(f'\u001B[1mUE IP Address for UE {self.module_name} is {ip}\u001B[0m')
		else:
			logging.debug(f'\u001B[1;37;41mUE IP Address for UE {self.module_name} Not Found!\u001B[0m')
		return ip

	def detach(self):
		self._command(self.cmd_dict["detach"])

	def check(self):
		cmd = self.cmd_dict["check"]
		if cmd:
			return self._command(cmd).stdout
		else:
			logging.warning(f"requested status check of UE {self.getName()}, but operation is not supported")
			return f"UE {self.getName()} does not support status checking"

	def dataEnable(self):
		cmd = self.cmd_dict["dataEnable"]
		if cmd:
			self._command(cmd)
			return True
		else:
			message = f"requested enabling data of UE {self.getName()}, but operation is not supported"
			logging.error(message)
			return False

	def dataDisable(self):
		cmd = self.cmd_dict["dataDisable"]
		if cmd:
			self._command(cmd)
			return True
		else:
			message = f"requested disabling data of UE {self.getName()}, but operation is not supported"
			logging.error(message)
			return False

	def getIP(self):
		output = self._command(self.cmd_dict["getNetwork"], silent=True)
		result = re.search('inet (?P<ip>[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)', output.stdout)
		if result and result.group('ip'):
			ip = result.group('ip')
			return ip
		return None

	def checkMTU(self):
		output = self._command(self.cmd_dict["getNetwork"], silent=True)
		result = re.search('mtu (?P<mtu>[0-9]+)', output.stdout)
		if result and result.group('mtu') and int(result.group('mtu')) == self.MTU:
			logging.debug(f'\u001B[1mUE Module {self.module_name} NIC MTU is {self.MTU} as expected\u001B[0m')
			return True
		else:
			logging.debug(f'\u001B[1;37;41m UE module {self.module_name} has incorrect Module NIC MTU or MTU not found! Expected: {self.MTU} \u001B[0m')
			return False

	def getName(self):
		return self.module_name

	def getIFName(self):
		return self.interface

	def getHost(self):
		return self.host

	def getNamespace(self):
		return self.namespace

	def getCNPath(self):
		return self.cnPath

	def getRunIperf3Server(self):
		return self.runIperf3Server

	def getCmdPrefix(self):
		return self.cmd_prefix if self.cmd_prefix else ""

	def _enableTrace(self):
		raise Exception("not implemented")

	def _disableTrace(self):
		raise Exception("not implemented")

	def _logCollect(self):
		raise Exception("not implemented")
