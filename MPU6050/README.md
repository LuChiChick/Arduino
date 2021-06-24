# 这是个什么？
## <font color=red size=4>MPU_6050.ino</font>
<font size=2>MPU6050是经典的一款陀螺仪，这个文件里面包含了Arduino封装的MPU6050的驱动库，并且封装成了MPU6050类，使用起来较为方便。  

<font size=1>~~因为是很早之前写的所以有些功能还未完善，比如防抖，以后有时间的话可能会向这里添加更多功能~~
</font>
</font>
---
## <font color=red size=4>visual_MPU.pde.ino</font>
<font size=2>应用Processing编写的一个应用MPU的例子，可以生成一个小方块并跟随MPU的倾角变化而变化

**使用须知**  
当然首先，你电脑上得有一个Processing  

这个例程是通过COM串口通信实现的，使用时需要更改里面的串口号到对应的Arduino设备，否则无法工作。
```Processing
.....
...........


void setup() {
  size (960, 640, P3D);
  //修改这里的COM8到你的Arduino串口
  myPort = new Serial(this, "COM8", 9600);
  myPort.bufferUntil('\n');
}
.....
...
..
```
</font>

---