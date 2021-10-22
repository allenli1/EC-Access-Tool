# **EC-Access-Tool**
A simple lightweight tool to read and write to the [Embedded Controller](https://en.wikipedia.org/wiki/Embedded_controller). 

## **Usage**

### **Windows (x64)**
```
Usage: EC-Access-Tool [options]

Options (per register operation):
        -[rwdrv|winring0]     Use RwDrv.sys or WinRing0.sys as the driver to access EC
        -w  [address] [value] Write to a register with 'address' and 'value'
        -r  [address]         Read from a register with 'address'

Example Usage (N operations at a time, N can be any natural number):
        EC-Access-Tool -winring0 -r RegAddr-1 -r RegAddr-2 -w RegAddr-3 WriteVal-3 ... -[w/r] RegAddr-N WriteVal-N
        Output: ReadVal-1 ReadVal-2 ... (Only the read outputs from N operations are displayed)

```
The input addresses/values and the output reading should be `1 byte` large and should be provided as a hex (max `0xFF`) string. Make sure that the driver is in the same repository as the tool. 

**Note:** This tool uses some sane defaults for internal `retries` and `timeout` measures. 
If the `read` operation times out, the string `0xXX` (an invalid hex value) will be printed.

### **Driver selection**
This tool can use either `WinRing0.sys` or `RwDrv.sys` windows driver. WinRing0 should work fine for most of the cases. 
The driver is also provided in the repo. However, as I have noticed occasional issues with WinRing0 on some Laptops, 
I have added support for `RwDrv`, the driver can be copied from `C:\Windows\System32\drivers` 
after installing [RWEverything](http://rweverything.com/). 

### **License**
* `EC-Access-Tool` is under `GPLv3` License, more on that can be found in [LICENSE](https://github.com/shubhampaul/EC-Access-Tool/blob/main/LICENSE).
* The base code is from the project [Embedded Controller](https://github.com/Soberia/EmbeddedController) under the `BSD 3-Clause` License, more on that can be found in [bsd-3-clause.LICENSE](https://github.com/shubhampaul/EC-Access-Tool/blob/main/bsd-3-clause.LICENSE).
* The interface code for RwDrv.sys is from the project [fwexpl](https://github.com/Cr4sh/fwexpl) under `GPLv3` License.

### **Disclaimer**
**Developer of `EC-Access-Tool` is not responsible for damage of any kind, please use it at your own risk.**
