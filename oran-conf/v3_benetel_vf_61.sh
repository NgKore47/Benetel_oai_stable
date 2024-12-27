set -x

sudo ethtool -G enp24s0f0 rx 8160
sudo ethtool -G enp24s0f0 tx 8160
sudo ifconfig enp24s0f0 mtu 9600

sudo modprobe -r iavf
sudo modprobe iavf
sudo sh -c 'echo 0 > /sys/class/net/enp24s0f0/device/sriov_numvfs'
sudo sh -c 'echo 2 > /sys/class/net/enp24s0f0/device/sriov_numvfs'
sudo ip link set enp24s0f0 vf 0 mac 00:11:22:33:44:61 vlan 3 qos 0 spoofchk off mtu 9600
sudo ip link set enp24s0f0 vf 1 mac 00:11:22:33:44:62 vlan 3 qos 0 spoofchk off mtu 9600

sudo /usr/local/bin/dpdk-devbind.py --unbind 0000:18:01.0
sudo /usr/local/bin/dpdk-devbind.py --unbind 0000:18:01.1
sudo modprobe vfio-pci
sudo /usr/local/bin/dpdk-devbind.py --bind vfio-pci 0000:18:01.0
sudo /usr/local/bin/dpdk-devbind.py --bind vfio-pci 0000:18:01.1

