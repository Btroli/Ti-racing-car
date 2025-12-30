### 主要更新
- 黑白线转换，现在线即是1
- 增加key123，但是只有3行

### 细节
- 微调八路显示
- oled一些细节
- LEFT RIGHT注意：00000111才是左，因为二进制从左往右读。


- 备份万用显示
```
    for (i = 0; i < 8; i++) {
        if ((ir_bits >> i) & 1)
            OLED_DrawBox(15 * i, 18, 12 + 15 * i, 30, 1);
        else
            OLED_DrawBoxLine(15 * i, 18, 11 + 15 * i, 29, 1);
        OLED_ShowChar(3 + 15 * i, 2, '1' + i, 12, 1);
    }
    OLED_Refresh();
    OLED_ClearRF();
```
