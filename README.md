# labcontroller
Проект по упралению лабораторным блоком питания через Wi-Fi.
Проект разделен на 2. Сделано это потому. что я не определился какая лучше. Само устройство представляет из себя блок питания от пк с контроллеров, в роли которого служит **ESP8266**
## Реализации
1. Версия прошивки [без RTOS](https://github.com/MrSyabro/labcontroller/tree/beta-1.0)
1. Версия более сложной прошивки [с  RTOS](https://github.com/MrSyabro/labcontroller/tree/beta-2.0)
## Прототипчик
На данный момент реализовано соединение контроллера с БП и сдвиговым регитром для управления периферией. 2 из возможных каналов регстра заняты светом и стабилизатором напряжения.
Макетная схема без стабилзатора выглядит примерно так:
![alt text](https://github.com/MrSyabro/labcontroller/raw/master/connections_bb.png)
