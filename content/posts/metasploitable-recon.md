---
date: 2026-07-13
categories:
  - Sicurezza Informatica
draft: true
tags:
  - Sicurezza Informatica
  - Metasploitable
  - Recon
title: "Metasploitable 2 — Information Gathering Report"

---

# Metasploitable 2 — Information Gathering Report

## Info Target

| Campo | Valore |
|-------|--------|
| **IP** | 192.168.56.143 |
| **MAC** | 52:54:00:1F:4A:80 (QEMU virtual NIC) |
| **OS** | Linux 2.6.9 - 2.6.33 (Ubuntu 8.04 Hardy) |
| **Hostname** | metasploitable.localdomain |
| **Distanza** | 1 hop (stessa LAN) |

---

## Porte Aperte (29 totali)

| Porta | Servizio | Versione | Severità |
|-------|----------|----------|----------|
| 21 | FTP | vsftpd 2.3.4 | Critica (backdoor) |
| 22 | SSH | OpenSSH 4.7p1 | Media |
| 23 | Telnet | Linux telnetd | Critica (non cifrato) |
| 25 | SMTP | Postfix smtpd | Media (SSLv2, VRFY) |
| 53 | DNS | ISC BIND 9.4.2 | Media |
| 80 | HTTP | Apache 2.2.8 + PHP 5.2.4 | Critica |
| 111 | RPCbind | v2 | Media |
| 139 | NetBIOS-SSN | Samba 3.0.20 | Critica (CVE-2007-2447) |
| 445 | Microsoft-DS | Samba 3.0.20-Debian | Critica (CVE-2007-2447) |
| 512 | exec | ? | Media |
| 513 | login | rlogin | Media |
| 514 | shell | rsh | Media |
| 1099 | Java RMI | GNU Classpath | Media |
| 1524 | bindshell | Metasploitable root shell | Critica (backdoor) |
| 2049 | NFS | v2-4 | Critica (root esportata) |
| 2121 | FTP | ProFTPD 1.3.1 | Media |
| 3306 | MySQL | 5.0.51a | Critica (root no password) |
| 3632 | distccd | v1 (GCC 4.2.4) | Critica (RCE) |
| 5432 | PostgreSQL | 8.3.0-8.3.7 | Media |
| 5900 | VNC | v3.3 | Media |
| 6000 | X11 | (access denied) | Bassa |
| 6667 | IRC | UnrealIRCd | Critica (backdoor) |
| 6697 | IRC | UnrealIRCd | Critica (backdoor) |
| 8009 | AJP13 | Apache Jserv | Media |
| 8180 | HTTP | Tomcat 5.5 | Media |
| 8787 | Ruby DRb RMI | Ruby 1.8 | Media |
| 34422 | nlockmgr | v1-4 | Bassa |
| 39132 | status | v1 | Bassa |
| 40630 | Java RMI | GNU Classpath | Media |
| 41260 | mountd | v1-3 | Bassa |

---

## Vulnerabilità Critiche

### 1. vsftpd 2.3.4 — Backdoor (CVE-2011-2523)
- **Porta:** 21
- **Tipo:** Backdoor inserita nel codice sorgente
- **Exploit:** Username con `:)` → backdoor su porta 6200
- **Impatto:** Shell come root
- **Fix:** Aggiornare a versione patched

### 2. Samba 3.0.20 — usermap_script (CVE-2007-2447)
- **Porta:** 139/445
- **Tipo:** Command execution via username injection
- **Exploit:** `username="/=\`nohup <cmd>\`"` → esecuzione come root
- **Impatto:** RCE senza autenticazione
- **Fix:** Aggiornare a >= 3.0.25rc3

### 3. UnrealIRCd — Backdoor (CVE-2010-2075)
- **Porta:** 6667/6697
- **Tipo:** Backdoor nel codice sorgente distribuito
- **Exploit:** Comandi con prefisso `AB` → esecuzione codice
- **Impatto:** Shell come utente IRC
- **Fix:** Aggiornare da fonte ufficiale

### 4. distccd v1 — Command Execution (CVE-2004-2687)
- **Porta:** 3632
- **Tipo:** Esecuzione comandi remoti senza autenticazione
- **Impatto:** Esecuzione codice come utente distccd
- **Fix:** Aggiornare o disabilitare

### 5. Bindshell porta 1524
- **Porta:** 1524
- **Tipo:** Backdoor root shell diretta
- **Impatto:** Accesso root immediato
- **Fix:** Rimuovere il servizio

### 6. NFS — Root esportata a tutti
- **Porta:** 2049
- **Config:** `/ *` → root filesystem esportato a qualsiasi IP
- **Impatto:** Montaggio remoto completo, accesso come root
- **Fix:** Restringere export a IP specifici, mai esportare `/`

### 7. MySQL — Root senza password
- **Porta:** 3306
- **Config:** root@`%` con password vuota
- **Impatto:** Accesso completo al DB da qualsiasi host
- **Fix:** Impostare password, bind su localhost

### 8. Web Server — Multipli vettori
- **Porta:** 80
- **phpinfo.php esposto** → info disclosure completa
- **phpMyAdmin esposto** → gestione DB senza restrizioni
- **WebDAV abilitato** → possibile upload file
- **TWiki** → vulnerabilità note
- **PHP 5.2.4** → CVE multiple
- **Apache 2.2.8** → CVE multiple
- **Header di sicurezza mancanti** → CSP, HSTS, X-Frame-Options assenti

---

## Vulnerabilità Medie

| Servizio | Problema |
|----------|----------|
| **SMTP** | SSLv2 attivo, cifari export-grade, cert scaduto 2010, VRFY attivo |
| **SSH** | OpenSSH 4.7p1 vecchio, algoritmi deboli (DSA) |
| **DNS** | BIND 9.4.2, versione esposta |
| **FTP anonimo** | Login anonimo permesso, share scrivibile |
| **SMB** | Share tmp rw per anonimi, firma SMB disabilitata |
| **Telnet** | Protocollo non cifrato, password in chiaro |
| **VNC** | Protocollo 3.3 vecchio, possibile brute-force |
| **PostgreSQL** | Versione 8.3 vecchia, cert scaduto |
| **rsh/rlogin/rexec** | Servizi Unix vecchi, spesso senza auth |
| **Java RMI** | Possibile deserialization exploit |
| **Tomcat 5.5** | Versione EOL, credenziali default possibili |
| **Ruby DRb RMI** | Esecuzione remota oggetti Ruby |

---

## Servizi Web (porta 80)

### Tecnologie
- Apache 2.2.8 (Ubuntu) DAV/2
- PHP 5.2.4-2ubuntu5.10
- WebDAV 2

### Path Trovati
| Path | Status | Note |
|------|--------|------|
| /phpinfo.php | 200 | Info disclosure completa |
| /phpMyAdmin/ | 301 | Gestione DB esposta |
| /dav/ | 301 | WebDAV abilitato |
| /twiki/ | 301 | Wiki con CVE note |
| /test/ | 301 | Directory di test |
| /cgi-bin/ | 403 | Directory CGI |
| /doc/ | 200 | Directory indexing attivo |
| /icons/ | 200 | Directory indexing attivo |

### Vulnerabilità Web
- HTTP TRACE attivo (XST)
- MultiViews abilitato (brute-force nomi file)
- Directory indexing abilitato
- Nessun header di sicurezza (CSP, HSTS, X-Content-Type-Options)
- PHP Easter Eggs accessibili

---

## Database

### MySQL (porta 3306)
- Accesso come root senza password da qualsiasi host
- Database trovati: dvwa, metasploit, mysql, owasp10, tikiwiki, tikiwiki195
- Utenti: root@`%` (no password), guest@`%` (no password), debian-sys-maint (no password)

### PostgreSQL (porta 5432)
- Versione 8.3.0-8.3.7
- Certificato SSL scaduto 2010

---

## SMB/NFS

### SMB (139/445)
- Accesso anonimo permesso
- Workgroup: WORKGROUP
- Firma SMB: disabilitata
- Share:
  - `print$` — driver stampanti
  - `tmp` — **rw per anonimi** (commento: "oh noes!")
  - `opt` — directory /opt
  - `IPC$`, `ADMIN$` — standard

### NFS (2049)
- Export: `/ *` → root filesystem a tutti
- Versioni: 2, 3, 4

---

## Riepilogo Superficie di Attacco

```
TOTALE: 29 porte aperte
├── Backdoor note:           1524, 21, 6667, 6697
├── RCE senza auth:          445 (Samba), 3632 (distccd)
├── Auth mancante/debole:    3306 (MySQL), 2049 (NFS), 21 (FTP anonimo)
├── Web attack surface:      80 (phpinfo, phpMyAdmin, WebDAV, TWiki)
├── Info disclosure:         53, 22, 25, 80, 5432
├── Servizi insicuri:        23 (Telnet), 512-514 (rsh suite)
└── Altro:                   5900 (VNC), 8180 (Tomcat), 8787 (Ruby DRb)
```

---

## Raccomandazioni (Come Difendersi)

### Priorità 1 — Critiche
1. **Chiudere porte non necessarie** — principio del minimo privilegio
2. **Patch Samba** → >= 3.0.25rc3 (CVE-2007-2447)
3. **Patch vsftpd** → versione patched (CVE-2011-2523)
4. **Rimuovere UnrealIRCd** → reinstallare da fonte ufficiale
5. **Rimuovere bindshell** porta 1524
6. **Patch distccd** o disabilitare
7. **MySQL** → impostare password root, bind su 127.0.0.1
8. **NFS** → restringere export, mai usare `*`, mai esportare `/`

### Priorità 2 — Alte
9. **Disabilitare FTP anonimo**
10. **Disabilitare Telnet** → usare SSH
11. **Disabilitare rsh/rlogin/rexec** → usare SSH
12. **phpMyAdmin** → proteggere con autenticazione, limitare IP
13. **Rimuovere phpinfo.php** da produzione
14. **Disabilitare WebDAV** se non necessario
15. **Aggiornare PHP** → versione supportata
16. **Aggiornare Apache** → versione supportata

### Priorità 3 — Medie
17. **Aggiungere header di sicurezza** → CSP, HSTS, X-Content-Type-Options, X-Frame-Options
18. **Disabilitare HTTP TRACE**
19. **Disabilitare MultiViews**
20. **Disabilitare directory indexing**
21. **SMTP** → disabilitare SSLv2, rimuovere VRFY, rinnovare certificato
22. **VNC** → tunnel su SSH, password forte
23. **PostgreSQL** → aggiornare, credenziali forti, bind localhost

### Monitoraggio
24. **Firewall** → iptables/nftables, bloccare tutto tranne servizi necessari
25. **IDS/IPS** → Snort o Suricata per rilevare scan e exploit
26. **Fail2ban** → bloccare brute-force su SSH, FTP, SMTP
27. **Log centralizzati** → rsyslog + SIEM
28. **Audit periodico** → scansioni Nmap regolari per verificare superficie di attacco
