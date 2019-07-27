
# Routerd Evaluation w/ tinet

```
               (1)
	 R1[net0]-----------[net0]R2
  [net1]                 [net1]
    |                      |
(2) |                      |  (3)
    |                      |
  [net0]                 [net0]
   R3[net1]-----------[net1]R4
	             (4)

lo: 10.255.0.N/32
(1): 10.1.0.N/24
(2): 10.2.0.N/24
(3): 10.3.0.N/24
(4): 10.4.0.N/24
```

## VPP runs on R1

