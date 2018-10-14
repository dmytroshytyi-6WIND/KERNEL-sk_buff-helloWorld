/**
 * @file    lkm.c
 * @author  Dmytro Shytyi
 * @date    14 Octobre 2018
 * @version 0.1
 * @brief  A "Hello World!" loadable kernel module (LKM) that sends Hello World udp packet.
 * @see https://dmytro.shytyi.net/ for a full description and follow-up descriptions.
*/
 
#include <linux/init.h>             // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/fcntl.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/skbuff.h>
#include <linux/if_arp.h>
#include <linux/netdevice.h> 
#include <linux/etherdevice.h>
#include <linux/netdevice.h> 
#include <linux/etherdevice.h>
//#include <linux/string.h>
#include <linux/ip.h> 
#include <linux/udp.h>
MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Dmytro Shytyi");      ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple linux kernel module - send hello world packet.");  ///< The description -- see modinfo
MODULE_VERSION("0.1");              ///< The version of the module
 
 
static int send_my(struct net_device* dev, uint8_t dest_addr[ETH_ALEN], uint16_t proto);

static int __init helloLKM_init(void){
	uint16_t proto;
	static char addr[ETH_ALEN] = {0xff,0xff,0xff,0xff,0xff,0xff};
	uint8_t dest_addr[ETH_ALEN];
	struct net_device *enp0s3;
	int a;
	enp0s3 = dev_get_by_name(&init_net,"enp0s3");
	memcpy (dest_addr, addr,ETH_ALEN);
	proto = ETH_P_IP;
	a = send_my(enp0s3,dest_addr,proto);
	printk(KERN_INFO "Hello from the  LKM!\n" );
   return 0;
}

unsigned int inet_addr(char *str)
{
    int a, b, c, d;
    char arr[4];
    sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d);
    arr[0] = a; arr[1] = b; arr[2] = c; arr[3] = d;
    return *(unsigned int *)arr;
}

int
send_my(struct net_device* dev, uint8_t dest_addr[ETH_ALEN], uint16_t proto)
{
  int            ret;
  unsigned char* data;
  
 char *srcIP = "192.168.0.1";
  char *dstIP = "192.168.0.2";
  char *hello_world = ">>> KERNEL sk_buff Hello World <<< by Dmytro Shytyi";
  int data_len = 51;

  int udp_header_len = 8;
  int udp_payload_len = data_len;
  int udp_total_len = udp_header_len+udp_payload_len;

  int ip_header_len = 20;
  int ip_payload_len = udp_total_len;
  int ip_total_len = ip_header_len + ip_payload_len;

/* skb */
  struct sk_buff* skb = alloc_skb(ETH_HLEN+ip_total_len, GFP_ATOMIC);//allocate a network buffer
  skb->dev = dev;
  skb->pkt_type = PACKET_OUTGOING;
  skb_reserve(skb, ETH_HLEN+ip_header_len+udp_header_len);//adjust headroom
/* allocate space to data and write it */
  data = skb_put(skb,udp_payload_len);
  memcpy(data, hello_world, data_len);
/* UDP header */
  struct udphdr* uh=(struct udphdr*)skb_push(skb,udp_header_len);
  uh->len = htons(udp_total_len);
  uh->source = htons(15934);
  uh->dest = htons(15904);

/* IP header */
  struct iphdr* iph=(struct iphdr*)skb_push(skb,ip_header_len);
  iph->ihl = ip_header_len/4;//4*5=20 ip_header_len
  iph->version = 4; // IPv4u
  iph->tos = 0; 
  iph->tot_len=htons(ip_total_len); 
  iph->frag_off = 0; 
  iph->ttl = 64; // Set a TTL.
  iph->protocol = IPPROTO_UDP; //  protocol.
  iph->check = 0; 
  iph->saddr=inet_addr(srcIP);
  iph->daddr=inet_addr(dstIP);

  /*changing Mac address */
  struct ethhdr* eth = (struct ethhdr*)skb_push(skb, sizeof (struct ethhdr));//add data to the start of a buffer
  skb->protocol = eth->h_proto = htons(proto);
  skb->no_fcs = 1;
  memcpy(eth->h_source, dev->dev_addr, ETH_ALEN);
  memcpy(eth->h_dest, dest_addr, ETH_ALEN);
 

  skb->pkt_type = PACKET_OUTGOING;
  ret = dev_queue_xmit(skb);
  return 1;
} 

static void __exit helloLKM_exit(void){
   printk(KERN_INFO "Goodbye from LKM!\n");
}
 
module_init(helloLKM_init);
module_exit(helloLKM_exit);
