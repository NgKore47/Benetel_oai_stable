# Benetel Testing Environment(E-release)

**Table of Contents**
1. [Hardware Info](#1-hardware-info)
2. [Configure the Server](#2-configure-the-server)
- 2.1. [Real-Time Kernel](#21-real-time-kernel)
- 2.2. [CPU Management](#22-cpu-management)
- 2.3. [Tuned Profile](#23-tuned-profile)
3. [DPDK(Data Plane Development Kit) 20.11.7](#3-dpdkdata-plane-development-kit-20117)
4. [OSC-PHY installation](#4-osc-phy-installation)
5. [Build OAI gNB](#5-build-oai-gnb)
6. [Linux PTP installation](#6-linux-ptp-installation)
7. [Network and vf Configuration](#7-network-and-vf-configuration)
8. [Benetel Configuration](#8-benetel-configuration)
9. [Run OAI gNB](#9-run-oai-gnb)
10. [Important Note](#10-important-note)
            
# 1. Hardware Info

### 1 NUMA Node system i.e server `seven`

|Hardware (CPU,RAM)                          |Operating System (kernel)                  |NIC (Vendor,Driver)                     | Server Number |
|--------------------------------------------|----------------------------------|-------------------------------------------------|------|
| Intel(R) Xeon(R) Gold 6354, 18-Core, 1 nodes | Ubuntu 22 RTK | Intel E810-XXV  | server 7 |


Bentel 550: `RAN550-1v1.0.4-605a25a`
# 2. Configure the Server

`In the BIOS`

* Turbo boost should be ON
* CPU Enhanced Halt State or C1E should be DISABLED
* Hyper-Threading should be OFF
* SRIOV should be ENABLED

## 2.1 Real Time Kernel

Install the real time Kernel

```shell
sudo pro attach <token>
sudo pro enable realtime-kernel
sudo reboot
```
After the reboot is done check your kernel

```shell
# uname -r

5.15.0-1054-realtime
```

## 2.2 CPU Management

|Applicative Threads|Allocated CPUs    |
|-------------------|------------------|
|XRAN DPDK usage    |0,2,4             |
|OAI `ru_thread`    |6                 |
|OAI `L1_rx_thread` |8                 |
|OAI `L1_tx_thread` |10                |
|OAI `nr-softmodem` |3,5,7,9,11,12,13,14|
|kernel             |14-17             |

Let's update the grub according to this configuration(plus hugepages will also be added)

Add the below paramters in the `GRUB_CMDLINE_LINUX` in the `/etc/default/grub` file
```shell
GRUB_CMDLINE_LINUX="intel_iommu=on iommu=pt mitigations=off mce=off idle=poll hugepagesz=1G hugepages=40 hugepagesz=2M hugepages=0 default_hugepagesz=1G selinux=0 enforcing=0 nmi_watchdog=0 softlockup_panic=0 audit=0 skew_tick=1 rcu_nocb_poll kthread_cpus=14-17 skew_tick=1 isolcpus=managed_irq,domain,0-13 nohz_full=0-13 rcu_nocbs=0-13 intel_pstate=disable nosoftlockup tsc=nowatchdog"
```

```shell
sudo update-grub
sudo reboot
```

After rebooting confirm the changes

```shell
# cat /proc/cmdline

BOOT_IMAGE=/boot/vmlinuz-5.15.0-1054-realtime root=UUID=d6117b44-2842-41f9-9657-c2131b8f8abb ro intel_iommu=on iommu=pt mitigations=off mce=off idle=poll hugepagesz=1G hugepages=40 hugepagesz=2M hugepages=0 default_hugepagesz=1G selinux=0 enforcing=0 nmi_watchdog=0 softlockup_panic=0 audit=0 skew_tick=1 rcu_nocb_poll kthread_cpus=14-17 skew_tick=1 isolcpus=managed_irq,domain,0-13 nohz_full=0-13 rcu_nocbs=0-13 intel_pstate=disable nosoftlockup tsc=nowatchdog
```
Change the cpu `governer` to `performance`

```shell
sudo apt install linux-tools-$(uname -r)
sudo cpupower frequency-set --governor performance
```

## 2.3 Tuned Profile

Real-time profile should be activated now:

Update the `/etc/tuned/realtime-variables.conf` file by adding the isolated cores

```shell
# cat /etc/tuned/realtime-variables.conf

isolated_cores=0-13
#
#
# Uncomment the 'isolate_managed_irq=Y' bellow if you want to move kernel
# managed IRQs out of isolated cores. Note that this requires kernel
# support. Please only specify this parameter if you are sure that the
# kernel supports it.
#
# isolate_managed_irq=Y
```

# 3. DPDK(Data Plane Development Kit) 20.11.7]

```shell
sudo apt install wget xz-utils libnuma-dev
cd
wget http://fast.dpdk.org/rel/dpdk-20.11.9.tar.xz
```
Compilation and Installation

```shell
sudo apt install meson
tar xvf dpdk-20.11.9.tar.xz && cd dpdk-stable-20.11.9
meson build
ninja -C build
sudo ninja install -C build
```
**Confirm the installation**
```shell
# sudo ldconfig -v | grep rte_
	librte_fib.so.0.200.2 -> librte_fib.so.0.200.2
	librte_telemetry.so.0.200.2 -> librte_telemetry.so.0.200.2
	librte_compressdev.so.0.200.2 -> librte_compressdev.so.0.200.2
	librte_gro.so.20.0 -> librte_gro.so.20.0.2
	librte_mempool_dpaa.so.20.0 -> librte_mempool_dpaa.so.20.0.2
	librte_distributor.so.20.0 -> librte_distributor.so.20.0.2
	librte_rawdev_dpaa2_cmdif.so.20.0 -> librte_rawdev_dpaa2_cmdif.so.20.0.2
	librte_mempool.so.20.0 -> librte_mempool.so.20.0.2
	librte_pmd_octeontx2_crypto.so.20.0 -> librte_pmd_octeontx2_crypto.so.20.0.2
	librte_common_cpt.so.20.0 -> librte_common_cpt.so.20.0.2
```

```shell
# pkg-config --libs libdpdk --static

-lrte_node -lrte_graph -lrte_bpf -lrte_flow_classify -lrte_pipeline -lrte_table -lrte_port -lrte_fib -lrte_ipsec -lrte_vhost -lrte_stack -lrte_security -lrte_sched -lrte_reorder -lrte_rib -lrte_rawdev -lrte_pdump -lrte_power -lrte_member -lrte_lpm -lrte_latencystats -lrte_kni -lrte_jobstats -lrte_ip_frag -lrte_gso -lrte_gro -lrte_eventdev -lrte_efd -lrte_distributor -lrte_cryptodev -lrte_compressdev -lrte_cfgfile -lrte_bitratestats -lrte_bbdev -lrte_acl -lrte_timer -lrte_hash -lrte_metrics -lrte_cmdline -lrte_pci -lrte_ethdev -lrte_meter -lrte_net -lrte_mbuf -lrte_mempool -lrte_rcu -lrte_ring -lrte_eal -lrte_telemetry -lrte_kvargs -Wl,--whole-archive -lrte_common_cpt -lrte_common_dpaax -lrte_common_iavf -lrte_common_octeontx -lrte_common_octeontx2 -lrte_bus_dpaa -lrte_bus_fslmc -lrte_bus_ifpga -lrte_bus_pci -lrte_bus_vdev -lrte_bus_vmbus -lrte_mempool_bucket -lrte_mempool_dpaa -lrte_mempool_dpaa2 -lrte_mempool_octeontx -lrte_mempool_octeontx2 -lrte_mempool_ring -lrte_mempool_stack -lrte_pmd_af_packet -lrte_pmd_ark -lrte_pmd_atlantic -lrte_pmd_avp -lrte_pmd_axgbe -lrte_pmd_bond -lrte_pmd_bnxt -lrte_pmd_cxgbe -lrte_pmd_dpaa -lrte_pmd_dpaa2 -lrte_pmd_e1000 -lrte_pmd_ena -lrte_pmd_enetc -lrte_pmd_enic -lrte_pmd_failsafe -lrte_pmd_fm10k -lrte_pmd_i40e -lrte_pmd_hinic -lrte_pmd_hns3 -lrte_pmd_iavf -lrte_pmd_ice -lrte_pmd_igc -lrte_pmd_ixgbe -lrte_pmd_kni -lrte_pmd_liquidio -lrte_pmd_memif -lrte_pmd_netvsc -lrte_pmd_nfp -lrte_pmd_null -lrte_pmd_octeontx -lrte_pmd_octeontx2 -lrte_pmd_pfe -lrte_pmd_qede -lrte_pmd_ring -lrte_pmd_sfc -lrte_pmd_softnic -lrte_pmd_tap -lrte_pmd_thunderx -lrte_pmd_vdev_netvsc -lrte_pmd_vhost -lrte_pmd_virtio -lrte_pmd_vmxnet3 -lrte_rawdev_dpaa2_cmdif -lrte_rawdev_dpaa2_qdma -lrte_rawdev_ioat -lrte_rawdev_ntb -lrte_rawdev_octeontx2_dma -lrte_rawdev_octeontx2_ep -lrte_rawdev_skeleton -lrte_pmd_caam_jr -lrte_pmd_dpaa_sec -lrte_pmd_dpaa2_sec -lrte_pmd_nitrox -lrte_pmd_null_crypto -lrte_pmd_octeontx_crypto -lrte_pmd_octeontx2_crypto -lrte_pmd_crypto_scheduler -lrte_pmd_virtio_crypto -lrte_pmd_octeontx_compress -lrte_pmd_qat -lrte_pmd_ifc -lrte_pmd_dpaa_event -lrte_pmd_dpaa2_event -lrte_pmd_octeontx2_event -lrte_pmd_opdl_event -lrte_pmd_skeleton_event -lrte_pmd_sw_event -lrte_pmd_dsw_event -lrte_pmd_octeontx_event -lrte_pmd_bbdev_null -lrte_pmd_bbdev_turbo_sw -lrte_pmd_bbdev_fpga_lte_fec -lrte_pmd_bbdev_fpga_5gnr_fec -Wl,--no-whole-archive -lrte_node -lrte_graph -lrte_bpf -lrte_flow_classify -lrte_pipeline -lrte_table -lrte_port -lrte_fib -lrte_ipsec -lrte_vhost -lrte_stack -lrte_security -lrte_sched -lrte_reorder -lrte_rib -lrte_rawdev -lrte_pdump -lrte_power -lrte_member -lrte_lpm -lrte_latencystats -lrte_kni -lrte_jobstats -lrte_ip_frag -lrte_gso -lrte_gro -lrte_eventdev -lrte_efd -lrte_distributor -lrte_cryptodev -lrte_compressdev -lrte_cfgfile -lrte_bitratestats -lrte_bbdev -lrte_acl -lrte_timer -lrte_hash -lrte_metrics -lrte_cmdline -lrte_pci -lrte_ethdev -lrte_meter -lrte_net -lrte_mbuf -lrte_mempool -lrte_rcu -lrte_ring -lrte_eal -lrte_telemetry -lrte_kvargs -Wl,-Bdynamic -pthread -lm -ldl
```

# 4. OSC-PHY installation
**E-release**

All the patches are already applied in the `phy` and available in the `phy` directory in this repo:

```shell
git clone https://github.com/NgKore47/Benetel_oai_stable.git
cd Benetel_oai_stable
```

Phy installation

```shell
cd phy
cd /fhi_lib/lib/
RTE_SDK=~/dpdk-stable-20.11.9/ XRAN_DIR=~/Benetel_oai_stable/phy/fhi_lib make XRAN_LIB_SO=1
```


# 5. Build OAI gNB

```shell
cd ~/Benetel_oai_stable
cd cmake_targets
./build_oai -I
./build_oai --gNB --ninja -t oran_fhlib_5g --cmake-opt -Dxran_LOCATION=$HOME/Benetel_oai_stable/phy/fhi_lib/lib
```

# 6. Linux PTP installation

```shell
git clone https://github.com/NgKore47/linuxptp.git
cd linuxptp
sudo make
sudo make install
```
Execute ptp4l and phc2sys for C3

```shell
sudo ./ptp4l -i enp24s0f0 -m -H -2 -s -f configs/benetel_e810.cfg
sudo ./phc2sys -w -m -s enp24s0f0 -R 8 -f configs/benetel_e810.cfg
```

# 7. Network and vf Configuration

Run the script

```shell
sudo bash ~/Benetel_oai_stable/oran-conf/v3_benetel_vf_61.sh
```


```shell
# cat v3_benetel_vf_61.sh

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
```

# 8. Benetel Configuration

Access the Benetel RU: `ssh root@10.10.0.100`

Make sure to match the following:

```shell
root@benetelru:~# cat /etc/ru_config.cfg

mimo_mode=1_2_3_4_4x4
downlink_scaling=0
prach_format=short
compression=static_compressed
lf_prach_compression_enable=true
cplane_per_symbol_workaround=disabled
cuplane_dl_coupling_sectionID=disabled
flexran_prach_workaround=disabled
dl_tuning_special_slot=0x13b6
```

Check the O-RU startup logs. It should match with `~/Benetel_oai_stable/bentel_ran_logs/benetel_starting_logs.log`

Check the oru_mac_vlan_info

```shell
root@benetelru:~# oru_vlan_mac_info
=============================================
DU VLAN and MAC address configured in O-RU:
=============================================

DU VLAN Tag Control Information for downlink C-Plane Traffic (C0331)
Register 0xc0331, Value : 0x3

DU VLAN Tag Control Information for downlink U-Plane Traffic (C0330)
Register 0xc0330, Value : 0x3

DU VLAN Tag Control Information for uplink U-Plane Traffic (C0318)
Register 0xc0318, Value : 0x3

 DU MAC Address for C-Plane Traffic (C0319/C031A)
Register 0xc0319, Value : 0x22334462
Register 0xc031a, Value : 0x11

DU MAC Address for U-Plane Traffic (C0315/C0316)
Register 0xc0315, Value : 0x22334461
Register 0xc0316, Value : 0x11
```

If mac addr doesn't match then run the following commands

```shell
registercontrol -w C0315 -x 0x22334461
registercontrol -w C0319 -x 0x22334462
```
**Note**: You need to run these `registercontrol` commands, everytime you reboot the Benetel RU
Then check `oru_vlan_mac_info` again

# 9. Run OAI gNB

```shell
cd ~/Benetel_oai_stable/cmake_targets/ran_build/build
sudo ./nr-softmodem -O ~/Benetel_oai_stable/oran-conf/e_release_benetel550.conf --thread-pool 3,5,7,9,11,12,13,14
```

PRACH and PUSCH values should be there.
After 20 seconds try to connect the UE now. 

# 10. Important Note

**Did the UE connect?** 

If yes, then this section is not for you. Good-Bye, Have a good day :)

If no, then first of all congratulations for reaching upto this point. It's great that the UE didn't connect because sometimes in life things don't end up the way we supposed it to be and all we can do is try harder instead of lying down. 

Let's try harder and remeber the above statement everytime you fail.

* First of all, remove all the other connections from your fibolan switch and use only two ports: one which is connectd to Benetel RU and other one is the DU server.
* Set vlan port 3(only) on these fibrolan ports and no other vlan id.
* Restart DU server
* After the restart is done, confirm the settings that are mentioned in above sections like tuned-adm, vf, mac addr and other required things
* Make sure linuxptp is working finr
* Once everything is set on DU, now restart the Benetel RU, and after the restart is done confirm the configurations as mentioned in the Bentel Configuration section.
* Make sure RU bringup is complete
* now run the nr-softmodem command and check prach and push values are appearing.
* Now wait for 20 seconds and then try connecting the UE, wait for few seconds and then UE will be connected(trying connecting with different-different UEs)
* Most important, have patience while connecting UE but dont waste much time (because there is a differnce between patience and wasting time :)

