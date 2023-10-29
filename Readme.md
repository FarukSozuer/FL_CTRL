
## STM32H7 Ucus Bilgisayari Yazilimi


### Yapilacaklar : 
1. CubeMX uzerinden konfigurasyon degisikligi yapildiginda 1.2.1 numarali CubeIDE versiyonunda proje c formatina donusebiliyor. Bu durumda degisiklikten sonra FL_CTRL_CM4 ve FL_CTRL_CM7 uzerine sag tiklayip "Convert to C++" secildiginde sorunsuz olarak derlenebiliyor.
1. master branchi acmadan once STM32CubeIDE v1.3.0 ve STM32Cube_FW_H7_V1.7.0 guncellemeleri yapilmali, daha sonra var olan workspace silinip programin yeniden olusturmasi saglanmalidir.
1. Not: FatFs kullanimi dolayisiyla M4 cekirdegi task yapisi icerisinde islem yukunde artislar gorulecektir. Bu yuzden ucus kritik veya gorev kritik herhangi bir islemin M4 cekirdeginde yapilmamasi gerekmektedir. Mevcut yapıda, M4 cekirdegi sadece depolama birimine veri kaydedilmesi veya depolama biriminden veri okunabilmesi icin kullanilmalidir.  
1. VN100 kutuphanesinde ASCII karakterlerle islemler yapildigi icin sprintf fonksiyonu kullanilmaktadir. Baska bir projede kullanilmak isteniyorsa ilgili proje dosyasinda properties sekmesinde C/C++ Build->Settings->Tool Settings altinda User float printf/scanf secili olmalidir. Aksi halde proje derlenyemecektir.

### Yapilan degisiklikler:


##### 11 Ocak 2021 @ Emre:
1. ds_irlock kütüphanesi eklendi.

##### 22 Aralık 2020 @ Alperen:
1. ds_venus kütüphanesi eklendi.


##### 24 Kasim 2020 @ Faruk:
1. ds_vn100 kutuphanesi eklendi.
1. ds_gnss kutuphanesi eklendi.
1. ds_vn100 icerisinde kullanilan ds_parse kutuphanesi eklendi.


##### 3 Temmuz 2020 @ Baris :
1. SDMMC1 M4 cekirdegi icin aktive edildi.
1. FatFs kutuphanesi M4 cekirdegi icin aktive edildi.
1. Secilecek verilerin, depolama biriminde olusturulan .DAT uzantili dosyaya yazilmasi saglandi.
1. Olusturulan .DAT uzantili dosya isminin GNSS'den alinan tarih-saat bilgisine gore belirlenebilmesi saglandi. 
1. Cekirdekler arasi haberlesmede SRAM 4 ve AXI_SRAM kullanildi.
1. Loglanacak veri boyutunun parametrik olarak ayarlanabilmesi saglandi.
1. Verilerin ne siklikla loglanacaginin parametrik olarak ayarlanabilmesi saglandi. 


##### 9 Haziran 2020 @ Onur :
1. Futaba SBUS2 surucusu telemetri destekli olarak olusturuldu.
1. Haberlesme icin Uart8, telemetri frame yapisi icin TIM13 aktive edildi.
1. Futaba T10J ve T14SG Kumandalari ile kanal bilgileri ve telemetri testleri gerceklestirildi.
1. Sbus icin Uart8 half-duplex modda PJ8 pinine ayarlandi.


##### 5 Haziran 2020 @ Baris :
1. STM32 System Bootloader entegrasyonu YKİ entegrasyonu hariç yapıldı.
1. System Bootloader'a geçiş için Option Bytes registerlarının güncellenebilmesi sağlandı.



##### 5 Haziran 2020 @ Gurtac :
1. INS ve Radar haberlesme alt yapisi eklendi.



##### 4 Haziran 2020 @ Gurtac :
1. CppCheck klasoru eklendi.



##### 29 Mayis 2020 @ Gurtac :
1. YKI ve Görev bilgisayari haberlesme alt yapisi eklendi.
1. Otonomi yapısı ile ilgili SonarQube ve CppCheck hatalari giderildi.



##### 4 Mayis 2020 @ Onur :
1. M4 cekirdegi task yapisi ve islem yuku olcme aktif hale getirildi.
1. M4 cekirdegi task takibi icin TIM4 800Hz de aktive edildi.
1. Cekirdekler arasi haberlesme yapisi SRAM 4 uzerinden 31.5kB cift yonlu buffer ve cekirdek aktif/pasif durumu kontrolu de eklenerek olusuturuldu.
1. Cekirdekler arasi haberlesme telemetri protokolu olusturuldu ve test edildi. Gonderilmek istenen paket telemetri yapisinda olusturulduktan sonra surekli paylasilabilecek sekilde duzenlendi.
1. M4 cekirdegi debug.printf fonksiyonu cekirdekler arasi haberlesme üzerinden M7 cekirdegi uart1 modulu (USB) veri gonderebilecek sekilde duzenlendi.
1. Eval board uzerinde bulunan 4 ledin 2'si M7 cekirdeginde 2'si M4 cekirdeginde olacak sekilde duzenlendi.
1. PWM uretme kanallarini ayirmak amaciyla, task icin kullanilan TIM3 ve TIM4 modulleri yerine TIM6 ve TIM7 modulleri aktive edildi.



##### 7 Nisan 2020 @ Gurtac :
1. MODE super class'ı ve DISARM, MANUAL, READY_TO_TAKE_OFF, TAKE_OFF, USER_INPUT, POSITION_HOLD, WAYPOINT, BRAKE, PRECISION, RETURN_TO_LAND, EMERGENCY_LAND, GNSS_DENIED, GNSS_DENIED_LAND, 
   RC_GPS ve ESCAPE subclass'ları oluşturuldu.
1. VEHICLE super class'ı ve DOGAN ve ALBATROS subclass'ları oluşturuldu.
1. Otonom mod geçişleri alt yapısı hazırlandı.
1. Uçuş modlarının alt yapıları oluşturuldu.



##### 18 Subat 2020 @ Onur :
1. STM32CubeIDE v1.3.0, STM32Cube_FW_H7_V1.7.0 ve CubeMX 5.6.0 guncelleme versiyonlari icin ReleaseNotes icerisinde belirtildigi sekilde proje ve workspace silinmis ve sifirdan olusuturulmus ve test edilmistir.



##### 19 Subat 2020 @ Faruk:
1. changeBaudRate fonksiyonu icerisindeki switch/case yapisi kaldirildi. Test edildi.



##### 18 Subat 2020 @ Onur :
1. Task yapisinda kullanilmak uzere TIM3 modulu 800Hz interrupt olusturacak sekilde duzenlendi ve aktive edildi.
1. debug_tools kutuphanesinde kullanilmak uzere us tick icin TIM2 ve ms tick icin TIM5 aktive edildi.
1. Zaman olcum fonksiyonlari overflow durumunu dikkate alacak sekilde duzenlendi.
1. H747-EVAL karti uzerinde yer alan ledler icin led kutuphanesi olusturuldu.



##### 14 Subat 2020 @ Faruk:
1. RAM hafizasinda yasanan doluluk durumunu duzeltebilme adina MCU'da kullanilan butun RAM bloklari linker dosyasina eklendi.
1. Bu RAM bloklari arasindan AXI_SRAM _D1 ile testler yapildi. Mevcut yazilimda bozulan birsey gozlemlenmedi.
1. Mevcut yazilimda Usage %57'den %14'e dustu.
1. NOT = Kod suanda eski halinde. Sadece RAM bloklari eklendi. Standart RAM DTCM (128 K) ile calisilmaya devam ediyor.



##### 13 Subat 2020 @ Faruk:
1. Butun seri port hatlari aktif edildi. 3 seri hat test edildi. 
1. Her bir UART hattinin interrupt rutini yazildi.
1. Her bir UART hattinin changeBaudRate fonksiyonu yazildi. 3 UART hatti icin test edildi.
1. getBaudRateError fonksiyonu eklendi.



##### 12 Subat 2020 @ Faruk:
1. UART1 modulu icin RunTime BaudRate degisimi eklendi ve test edildi.
1. BaudRate degisiminde hata durumlarini gozlemlemek icin "change_baud_progress" ve "change_baud_progress_counter" degiskenleri eklendi ve test edildi.
1. changeBaudRate fonksiyonu bool tipinde tanimlanarak hata durumlari return edildi.



##### 10 Subat 2020 @ Onur :
1. UART1 modulu kutuphanede receive ve transmit interruptlari ile eklendi. Test edildi.
1. debug.printf UART1 ST link virtual com port uzerinden calisacak sekilde aktive edildi.



##### 10 Subat 2020 @ Onur :
1. CM7 Ana task yapisi icin TIM5 4kHz olarak ayarlandi ve task yapisina baglantisi yapildi.
1. CM7 Islemci yuk durumu izleme icin TIM2 1Mhz ve 2^32 tick sonrasinda kesme uretecek sekilde ayarlandi.
1. CM7 Ana task yapisi islemci yuk durumu etkinlestirildi.
1. Uart interface class ve h747 class taslak yapisi olusturuldu.
1. Can interface class ve h747 class taslak yapisi olusturuldu.



##### 6 Subat 2020 @ Onur :
1. Performans izleme icin 1Mhz Timer konfigurasyonu yapildi.



##### 5 Subat 2020 @ Onur :
1. CM7 Ana task yapisi olusturuldu.
