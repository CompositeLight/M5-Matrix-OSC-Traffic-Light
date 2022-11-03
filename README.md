
# M5-Matrix-OSC-Traffic-Light

Important note: Use at your own risk - I'm a rookie programmer. Any good bits in this code have been magpied from the Genius of Joseph Adams, as I've used his M5 Tally Arbiter listener as a starting point.

# Setup:
1) In Arduino, edit the sketch to insert your wifi SSID name and password. You also need to insert a manual IP address and subnet mask.
2) Once uploaded, the M5 will attempt to connect to wifi on power up.


# Operation:
On power up the M5 will attempt to connect to wifi. You will see a blue cross until it connects.
On network connection it will respond to OSC messages sent from any program - such as QLab.
If using QLab, set the network destination IP to match the IP you've set in the sketch.

Possible messages:

/red
/green
/yellow
/orange
/blue
/purple
/black
