# About this kernel module
In this repo you may find a small demo of the Linux Kernel Module (LKM) develloped for kernel 4.4.0-116-generic. It is sending udp packet with "Hello World" payload.

Use

```make``` 

and


```make clean```

to compile and clean the project in the cli of current directory.

To import/remove the module, type in the cli:

```insmod lkm.ko```

```rmmod lkm.ko```

When imported, the module will send 1 udp packet with helloworld payload broadcasting on the link layer and using ipv4 192.168.0.1(2) addresses.

LKM uses sk_buff in the Linux kernel

