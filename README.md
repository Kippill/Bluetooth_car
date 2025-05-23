"# Bluetooth_car" 
Задачей моего проекта являлось создание машинки Bots особенностью, которой должны были быть колеса формы треугольника Рело и камера, передающая изображение на экран устройства, с которого осуществляется управление. Решение использовать треугольные колеса вместо других альтернатив (гусениц, шнеков) было обусловлено скорее минутной слабостью и ошибкой нежели холодным расчетом, так как в момент когда я принял решение делать колеса формы треугольника Рело я почему-то думал, что для такой геометрической фигуры, как треугольник Рело, расстояние от оси, на которую крепятся колеса, до поверхности остается всегда постоянным. Это очевидно оказалось не так, но сама идея использовать треугольные колеса показалось мне довольно интересной, поэтому я решил не менять концепцию. Использование же камеры, как ещё одной особенности моего проекта, не казалось мне чем-то сложным, и я не ожидал столкнуться с теми трудностями, с которыми я столкнулся позднее, но в конечном итоге (с небольшой хитростью и немного отойдя от первоначальной задумки) я смог реализовать эту идею. Было написано два рабочих кода: один обеспечивает управление моторами, а другой выводит изображение с камеры. В конечном счёте данная машинка оказалось способной ездить на треугольных колесах и пеедавать изображение с камеры.

Компоненты которые использовались в сборке:
  - микроконтроллер ESP32 DEV MODULE
  - драйвер для моторов
  - силовой ключ
  - монтажная плата
  - два мотора
  - два держателя для аккумуляторов
  - пять аккумуляторов
  - корпус, выполненный из фанеры
  - треугольные колеса из TPU
  - микроконтроллер ESP32 CAM
  - Power Cell (Troyka модуль)
  - Аккумулятор ET ICR16340C Li-Ion / 3,7 В / 700 мА·ч
  - Светодиодная RGB матрица 4×4 (Troyka-модуль)
  - Соединительные провода
