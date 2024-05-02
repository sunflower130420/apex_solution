# Apex Solution
Apex Solution is a KVM (Kernel Virtual Machine) project that enables users to read physical memory through Memflow. If you’re interested in memory introspection and low-level system access, this project might be just what you’re looking for!

Usage
Installing KVM for Linux
Before using Apex Solution, you’ll need to set up KVM on your Linux system. Here’s how:

Update your package list:
sudo apt update

Install the necessary packages:
sudo apt -y install qemu-kvm libvirt-dev bridge-utils libvirt-daemon-system libvirt-daemon virtinst bridge-utils libosinfo-bin libguestfs-tools virt-top

Load and enable the vhost-net module (required for KVM networking):
# Load the module
sudo modprobe vhost-net

# For host/guess connection
sudo apt update
sudo apt install openssh-client

# Enable it to load at startup
echo "vhost-net" | sudo tee -a /etc/modules-load.d/vhost-net.conf

Installing Memflow
Now that KVM is set up, let’s install Memflow:

Install Memflow using memflowup (a setup tool for Memflow components):
memflowup install -d memflow-qemu memflow-coredump
This command installs the necessary connectors for Memflow. The -d flag specifies the development channel, which ensures you get the latest versions.
Install the Apex Solution connector:
memflowup install memflow-kvm -s -d --reinstall
This command installs the Apex Solution connector. The -s flag ensures that it’s installed system-wide, and --reinstall ensures any existing installation is updated.
Inspired by MisterY52/apex_dma_kvm_pub
Apex Solution was inspired by the work in the repository MisterY52/apex_dma_kvm_pub. Feel free to explore both projects and learn from their implementations!
