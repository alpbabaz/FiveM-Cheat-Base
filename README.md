<p align="center">
  <img src="https://img.shields.io/badge/status-active-brightgreen" alt="Status"/>
  <img src="https://img.shields.io/badge/platform-Windows_11-blue" alt="Platform"/>
  <img src="https://img.shields.io/badge/build-Release_x64-orange" alt="Build"/>
  <img src="https://img.shields.io/badge/framework-DirectX11_ImGui-9cf" alt="Framework"/>
</p>

<h1 align="center">FiveMAlpbase</h1>
<p align="center">
  <b>GTA V (FiveM) external-overlay cheat base</b><br/>
  DirectX 11 + ImGui · ESP · Aimbot · Silent Aim · Safe Destruct
</p>

<p align="center">
  <i>signed with <b>alp/fy0w</b></i>
</p>

---

## İçindekiler
- [Özellikler](#özellikler)
- [Derleme](#derleme)
- [Kurulum](#kurulum)
- [Kontroller](#kontroller)
- [Güvenlik / Gizlilik](#güvenlik--gizlilik)
- [Notlar](#notlar)

---

## Özellikler

### ☑ Aimbot
- Aimbot (hareket eden hedeflere kilitlenme)
- Aimbot FOV göstergesi
- Kemik seçimi (baş / gövde / uzuv)
- Sadece görünür hedefler, Ped atla, Ölü atla
- **Trigger Bot** — nişangah hedef üstündeyken otomatik ateş
- **Silent Aim** — mermi yeniden yönlendirme
- **Random Silent** — 2/5 mermi isabet

### 👤 ESP (Görüş Zararları)
- Oyuncu isimleri
- İskelet ESP
- Kutu ESP (4 tip + seçilebilir boyut)
- Can / Zırh barı
- Çizgi ESP (snap-lines, tip seçimi)
- Silah ESP
- Mesafe ESP
- Yön ESP
- Gradyan kutu efekti (5 farklı gradyan + özel renk, yoğunluk ayarı)
- Görünürlük / Ped / Ölü filtreleri

### 🚗 Vehicle ESP
- Araç ESP aç/kapat
- Yerel & düşman araç ayrımı
- Mesafe, işaretçi (marker), snapline, araç canı

### ⚙ Misc (Yardımcılar)
- No Clip (duvardan geçme)
- Vehicle Fix (araç tamiri)
- Unlock All Vehicles (tüm araç kilit açma)
- God Mode (3 sn akıllı geri yükleme)
- Health / Armor Boost
- FOV Changer
- Infinite Ammo
- No Recoil / No Spread / No Reload
- Damage Boost / Reload Ammo
- Teleport to Waypoint

### 🎨 Overlay & Arayüz
- DirectX 11 tam ekran overlay (ImGui)
- Custom ImGui widget'ları (AmGui)
- Watermark (FPS + key bilgisi)
- Streamproof (kayıt/yayından gizleme)
- VSync seçeneği
- Splash / animasyon ekranı + duman efekti

### 🛡 Güvenlik / Gizlilik
- **Safe Destruct** — tek tuşla izleri temizler, ekranı kapatır, OBS'yi öldürür
- String cleaner — araç belleğindeki marka/iz stringlerini sıfırlar
- PowerShell geçmişi ve Recent kısayolları temizliği
- Blacklist proses kontrolü (anti-screenmirror / anti-log)
- WMI / donanım kimliği toplama

---

## Derleme

**Gereksinimler:**
- Visual Studio 2022 (v143 toolset)
- Windows 10/11 x64
- Windows SDK + DirectX SDK kurulu

**Adımlar:**
1. Repoyu klonla.
2. `FiveM_Alpbase.sln` dosyasını Visual Studio 2022 ile aç.
3. **Release | x64** konfigürasyonunu seç.
4. **Build → Build Solution (Ctrl+Shift+B)**.

Çıktı: `x64\Release\obs-text.dll`

> Proje henüz bir **Auth Server** eklenmeden derlendiğinde çalıştırma aşamasına bağlıdır (aşağıdaki Notlar bölümüne bakınız).

---

## Kurulum

1. Derlenen `obs-text.dll` dosyasını FiveM'in bulunduğu makineye kopyala.
2. Bir injector ile GTA V (FiveM) prosesine enjekte et.
3. Konsol açılır, **Shift + F6** ile key'ini (:license_key) çağır.
4. Key doğrulaması yapılır — başarılıysa overlay açılır.

---

## Kontroller

| Tuş | Eylem |
|------|-------|
| **Shift + F6** | Session key göster / konsol |
| **F10** | Cheat'ı kapat / kaldır |
| **INSERT / Menu Key** | Menüyü aç / kapat |
| **Safe Destruct butonu** | İzleri temizle + overlay'i kapat |

> Kapatma tuşları `Source.cpp` üzerinden `GetAsyncKeyState` bloklarında özelleştirilebilir.

---

## Güvenlik / Gizlilik

Bu repo, **bireysel eğitim ve kişisel kullanım** için temizlenmiş bir taban olarak yayınlanmıştır.

- ❌ Webhook URL'leri — **silindi** (boş placeholder)
- ❌ Code-signing sertifikası (`.pfx`) — **repo'ya dahil edilmedi**
- ❌ AES / XOR obfuscation katmanları — **statik string'lere çevrildi**
- ❌ Hardcoded callback sunucusu — **placeholder ile değiştirildi**

Kendi sunucu / lisans altyapını eklerken `mjLib/mjLib.hpp` (Network) ve `Source.cpp` (auth akışı) bölümlerini düzenlemen yeterli.

---

## Notlar

- Bu proje **yasal amaçlar / özel sunucularda deneme** için hazırlanmıştır. Çevrimiçi sunucularda kullanım ilgili sunucunun kurallarına tabidir; sorumluluk kullanıcıya aittir.
- **GTA V** ve **FiveM** ilgili hakların sahiplerinin markalarıdır.
- Ticari olmayan kişisel gelişim amaçlıdır.

---

## Lisans & İmza

<p align="center">
  <b>Android · Libra · Phantom</b><br/>
  <code>alp/fy0w</code>
</p>

«alp/fy0w»
