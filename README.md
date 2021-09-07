# ESPSensorModule  

Переходим по [ip esp устройства] и видим список кнопок, далее описание каждой:  

**Registration** - добавление ip обратившегося клиента в callback для оповещения от датчика движения  
**Info** - информация по текущей температуре, влажности, активности датчика движения и т.д.  
**Setting** - установка отступа для значений температуры и влажности т.к. на работу датчика может влиять нагрев самой esp (как произвести настройку написано на странице)  
**Restart** - программный перезапуск системы  
**Metrics page** - страница с метриками, пока информация о кол-во перазапусков  

Для запуска также необходим файл в корневой директории **data.h** со следующим содержанием:  
```
const char* ssidValue = "название wifi сети";  
const char* passwordValue = "пароль wifi сети";  
const char* nameDeviceValue = "название устройства для прошивки по wifi";  
const char* passDeviceValue = "пароль для прошивки по wifi";  
```

### Схема системы
![Схема](https://github.com/HoriFox/SmartHomeCentralDoc/blob/main/gitimg/system.png)
