# FamicomWirelessController
Проект беспроводного геймпада для Famicom клонов (Dendy, Subor, Lifa, ...)

- Печатная плата подходит в большинство "классических" стандартных геймпадов с турбокнопками.
- Возможна одновременная работа двух и более гейпадов, в коде приемника и передатчика необходимо задать одинаковые адреса для каждой пары приемник-передатчик.
- По умолчанию передатчик геймпада настроен на "среднюю" мощность передачи. В пределах жилого помещения этого более, чем достаточно.
- В схеме геймпада имеются некоторые конструктивные недостатки. В режиме сна геймпад отключает передатчик и уходит в режим halt контроллера STM8, так как этот режим является энергосберегающим только условно, аккумулятор на 180-200мАч садится примерно за месяц. Возможно, когда-нибудь добавлю схему автоблокировки питания. Еще одним недостатком будет не очень удобное расположение индикатора работы, его можно не ставить.
- Для сборки прошивок необходимо установить IAR Embedded Workbench for STM8 v1.40.1 или залить готовые файлы прошивок через ST Visual Programmer
- После установки аккумулятора лучше всего стартануть геймпад через разъем программирования (подключить-отключить) или вручную запустить плату контроллера заряда путем замыкания "-" и "bat-" (или + и bat+)
- работа 9 контактного разъема приемника НЕ ПРОВЕРЕНА! Поэтому за правильную работу не ручаюсь.
- Описание и репозиторий будет пополняться, все недостающие файлы залью позже



A project of a wireless gamepad for famicom and its clones with 9 and 15 pin controller sockets.

