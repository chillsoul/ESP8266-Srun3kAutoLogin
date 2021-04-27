# ESP8266-Srun3kAutoLogin
基于ESP8266 NodeMCU 1.0的深澜校园网自动登录。

## 已知问题
~~部分数字加密后由于未经urlencode可能导致报密码错误，建议修改密码，有能力者可自行编写urlencode方法后pull request~~

已更新URLEncode，不会真的有倒霉蛋连学号都要编码吧？不会吧不会吧？

# 使用方法
1. 安装[Arduino](https://www.arduino.cc/en/Main/Software)

2. Arduino->首选项->设置->附加的开发板管理器网址
填入 `http://arduino.esp8266.com/stable/package_esp8266com_index.json`

3. Arduino->工具->开发板->开发板管理器->搜索ESP8266安装

4. 连接ESP8266开发板，安装驱动，选择端口（百度/谷歌）

5. 修改代码填入校园网账号后，将此固件刷入ESP8266即可。

![Running Screenshot](https://s1.ax1x.com/2020/09/21/wqdCvD.png)

> 本项目参考学习了[EHAUT](https://github.com/ehaut)内众多项目的代码，其他学校请自行修改。
