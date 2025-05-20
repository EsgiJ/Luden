@echo off
REM Önceki çıktı dosyasını sil (hata mesajını gizle)
del allcode.txt 2>nul

REM Geçerli klasör ve alt klasörlerdeki tüm .cpp dosyalarını allcode.txt'ye ekle
for /R %%f in (*.cpp) do type "%%f" >> allcode.txt

REM Geçerli klasör ve alt klasörlerdeki tüm .h dosyalarını allcode.txt'ye ekle
for /R %%f in (*.h)   do type "%%f" >> allcode.txt

echo Done!.
pause
