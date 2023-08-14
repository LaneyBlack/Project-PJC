# Project-PJC
Image steganography - Semester Project on Basics of C (PJATK)

2nd semester project

Project works only with .png and .bmp files.

This project features all those commands:
![image](https://github.com/LaneyBlack/Project-PJC/assets/44290162/5a597e33-a211-4267-a750-49995bcaca33)

--info:
In an image without ecnrypted message
![image](https://github.com/LaneyBlack/Project-PJC/assets/44290162/cd7a2ce4-30d4-4bca-b738-7fc12a0d1a8f)
I am putting a flag after file configuration bytes end. By the flag I can tell an existance of a message.

--decrypt:
![image](https://github.com/LaneyBlack/Project-PJC/assets/44290162/5949c842-c411-4a45-a5fd-4a76d501ab87)

--ecnrypt works the same way, but you should input the text for encryption in quotation.
![image](https://github.com/LaneyBlack/Project-PJC/assets/44290162/92a458e0-e2f9-489f-9c43-1d53270fe34e)
As we can see both images are identical. Sometimes if the text is very large you can see some missing pixels in the end of an image.
![image](https://github.com/LaneyBlack/Project-PJC/assets/44290162/8b24cf31-5299-4a85-90fe-3b575bb044f1)

--check stands for checking is an image suitable for this text. Is there enough space in an image to encrypt this text.
![image](https://github.com/LaneyBlack/Project-PJC/assets/44290162/471ebafd-65e3-4130-9006-782e09bde25f)

Also I could have implemented an algorithm that would choose per how many bites should I encrypt a message. And Leave this number right beside flag mark. 
