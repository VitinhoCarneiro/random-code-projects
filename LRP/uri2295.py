#!/usr/bin/env python3
(price_eth, price_gas, eff_eth, eff_gas) = list(map(float, input().split()))
if(eff_eth/price_eth > eff_gas/price_gas): print("A")
else: print("G")
