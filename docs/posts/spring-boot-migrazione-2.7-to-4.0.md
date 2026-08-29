---
title: "Guida alla Migrazione: Spring Boot 2.7 → 4.0"
date: 2026-04-24
description: "Checklist completa per migrare un progetto Spring Boot da 2.7 a 4.0 passando per 3.x"
categories:
  - Java
draft: true
---

# Guida alla Migrazione: Spring Boot 2.7 → 4.0

> **Consiglio**: non saltare direttamente a 4.0. Fai prima la migrazione a 3.5 (l'ultimo 3.x), verifica che funzioni, poi passa a 4.0.

---

## Tabella Riassuntiva

| | 2.7 | 3.x | 4.0 |
|---|---|---|---|
| Java minimo | 8 | 17 | 17 (21 raccomandato) |
| Kotlin minimo | 1.x | 1.x | 2.2+ |
| GraalVM native | sperimentale | AOT ufficiale | v25+, maturo |
| Spring Framework | 5.x | 6.x | 7.x |
| Jakarta EE | 8 (`javax.*`) | 9+/10 (`jakarta.*`) | 11 (`jakarta.*`), Servlet 6.1 |
| Hibernate | 5.x | 6.x | 7.x |
| Jackson | 2.x | 2.x | **3.x** (2.x deprecato) |
| Tomcat | 9 | 10.1 | 11 |
| Jetty | 9/10 | 11 | 12.1 |
| Undertow | supportato | supportato | **RIMOSSO** |
| Security config | `WebSecurityConfigurerAdapter` | `SecurityFilterChain` bean | Spring Security 7 |
| Auto-config | `spring.factories` | imports file | imports file (modulare) |
| Stato | EOL | Attivo (3.5) | Attivo (4.0.5 GA) |

---

## FASE 1: Pre-migrazione (su 2.7)

### 1. Aggiorna all'ultima 2.7.x

Assicurati di essere sull'ultima patch di 2.7 prima di qualsiasi altra cosa.

### 2. Risolvi tutte le deprecation

Compila con `-deprecation` e risolvi **tutti** i warning. Tutto ciò che è deprecato in 2.7 sarà **rimosso** in 3.x.

### 3. Aggiorna Spring Security a 5.8

Se usi Spring Security, aggiorna prima a 5.8 (compatibile con 2.7). Questo ti prepara al salto a 6.x:

- Rimuovi `WebSecurityConfigurerAdapter`
- Usa `SecurityFilterChain` bean-based
- Aggiorna la configurazione SAML2 (da `identity-provider` a `asserting-party`)

---

## FASE 2: Migrazione da 2.7 a 3.x (3.5 raccomandato)

### 4. Java 17+ obbligatorio

```xml
<!-- pom.xml -->
<properties>
    <java.version>17</java.version>
</properties>
```

```groovy
// build.gradle
sourceCompatibility = '17'
```

### 5. `javax.*` → `jakarta.*` (IL CAMBIO PIÙ IMPATTANTE)

Cerca e sostituisci in **tutto** il codice:

```java
// PRIMA (2.7)
import javax.servlet.*;
import javax.persistence.*;
import javax.validation.*;
import javax.annotation.*;
import javax.transaction.*;

// DOPO (3.x / 4.0)
import jakarta.servlet.*;
import jakarta.persistence.*;
import jakarta.validation.*;
import jakarta.annotation.*;
import jakarta.transaction.*;
```

Anche le dipendenze Maven/Gradle cambiano:

```xml
<!-- PRIMA -->
<dependency>
    <groupId>javax.servlet</groupId>
    <artifactId>javax.servlet-api</artifactId>
</dependency>

<!-- DOPO -->
<dependency>
    <groupId>jakarta.servlet</groupId>
    <artifactId>jakarta.servlet-api</artifactId>
</dependency>
```

**Strumenti utili**:
- **OpenRewrite**: `org.openrewrite.java.spring.JavaxMigrationToJakarta`
- **IntelliJ IDEA**: refactoring automatico
- **Spring Boot Migrator**: progetto sperimentale

### 6. Aggiorna `spring.factories` → imports file

Le auto-configurazioni non si registrano più in `spring.factories`:

```
# VECCHIO: META-INF/spring.factories
org.springframework.boot.autoconfigure.EnableAutoConfiguration=\
  com.example.MyAutoConfiguration

# NUOVO: META-INF/spring/org.springframework.boot.autoconfigure.AutoConfiguration.imports
com.example.MyAutoConfiguration
```

### 7. Proprietà rinominate/rimosse (2.7 → 3.x)

Aggiungi il migrator temporaneamente:

```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-properties-migrator</artifactId>
    <scope>runtime</scope>
</dependency>
```

Principali proprietà cambiate:

| 2.7 | 3.x |
|---|---|
| `spring.redis.*` | `spring.data.redis.*` |
| `spring.data.cassandra.*` | `spring.cassandra.*` |
| `server.max-http-header-size` | `server.max-http-request-header-size` |
| `httptrace` endpoint | `httpexchanges` endpoint |
| `spring.jpa.hibernate.use-new-id-generator-mappings` | **rimosso** |

### 8. Hibernate 5 → 6

- Nuovo groupId: `org.hibernate.orm` (non più `org.hibernate`)
- Molte query HQL/JPQL possono rompersi per cambiamenti nel parser
- Rimossa `spring.jpa.hibernate.use-new-id-generator-mappings`

### 9. Spring Security 5.x → 6.x

- `WebSecurityConfigurerAdapter` → `SecurityFilterChain` bean-based
- Authorization applicata a **ogni dispatch type** (non solo REQUEST)
- `spring.security.filter.dispatcher-types` per configurare i dispatch types

### 10. Altre rimozioni in 3.x

| Rimosso | Alternativa |
|---|---|
| Apache ActiveMQ | Usa Artemis |
| Atomikos | Usa Bitronix o Narayana |
| EhCache 2 | Usa EhCache 3 con classifier `jakarta` |
| Hazelcast 3 | Aggiorna a Hazelcast 5 |
| Apache Solr | Non supportato (Jetty incompatibile) |
| Image banner (`banner.gif/jpg/png`) | Usa `banner.txt` |
| `YamlJsonParser` | Usa altro `JsonParser` |
| `@ConstructorBinding` a livello di classe | Rimuovi l'annotazione |
| Trailing slash matching | Aggiungi `@GetMapping("/path", "/path/")` o configura `setUseTrailingSlashMatch(true)` |
| JMX Actuator: tutti gli endpoint esposti | Solo `health` esposto di default |
| Actuator sanitization: solo chiavi sensibili | Valori **sempre mascherati** di default |

### 11. Testa tutto su 3.5 prima di proseguire

Verifica che applicazione, test e integrazioni funzionino correttamente sulla versione 3.5 prima di passare a 4.0.

---

## FASE 3: Migrazione da 3.x a 4.0

### 12. Spring Framework 6.x → 7.x

- `org.springframework.lang.Nullable` → `org.jspecify.annotations.Nullable`
- Nuove JSpecify nullability annotations possono rompere la compilazione con null checker
- `@Autowired` obbligatorio nei costruttori di `@ConfigurationProperties` con dipendenze

### 13. Modularizzazione (IL CAMBIO PIÙ GRANDE IN 4.0)

Spring Boot 4 è stato **modularizzato**: `spring-boot-autoconfigure` è stato spezzato in ~60 moduli.

#### Package cambiati

```
# 3.x
org.springframework.boot.autoconfigure.web.servlet.*
org.springframework.boot.autoconfigure.data.jpa.*

# 4.0
org.springframework.boot.webmvc.*
org.springframework.boot.data.jpa.*
```

#### Starter rinominati

| 3.x | 4.0 |
|---|---|
| `spring-boot-starter-web` | `spring-boot-starter-webmvc` |
| `spring-boot-starter-oauth2-client` | `spring-boot-starter-security-oauth2-client` |
| `spring-boot-starter-oauth2-authorization-server` | `spring-boot-starter-security-oauth2-authorization-server` |
| `spring-boot-starter-oauth2-resource-server` | `spring-boot-starter-security-oauth2-resource-server` |
| `spring-boot-starter-web-services` | `spring-boot-starter-webservices` |
| `spring-boot-starter-aop` | `spring-boot-starter-aspectj` |

#### Nuovi starter dedicati (prima bastava la dipendenza diretta)

Ogni tecnologia ha ora uno starter dedicato (+ test starter):

```xml
<!-- PRIMA: bastava la dipendenza Flyway diretta -->
<dependency>
    <groupId>org.flywaydb</groupId>
    <artifactId>flyway-core</artifactId>
</dependency>

<!-- DOPO: serve lo starter -->
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-flyway</artifactId>
</dependency>
```

#### Strategia di migrazione graduale: "Classic Starters"

Per non rompere tutto subito, puoi usare i classic starters temporaneamente:

```xml
<!-- Temporaneo per migrare senza rompere tutto -->
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-classic</artifactId>
</dependency>
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-test-classic</artifactId>
    <scope>test</scope>
</dependency>
```

Questi includono tutto come nella 3.x. Poi gradualmente sostituisci con i moduli specifici.

> **Importante**: rimuovi i classic starters una volta completata la migrazione.

#### Security test

Se usi `@WithMockUser` o `@WithUserDetails`, ora serve:

```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-starter-security-test</artifactId>
    <scope>test</scope>
</dependency>
```

### 14. Jackson 2 → Jackson 3

Jackson 3 è il nuovo default. Cambia namespace:

```java
// PRIMA (2.7 / 3.x)
com.fasterxml.jackson.databind.ObjectMapper

// DOPO (4.0)
tools.jackson.databind.ObjectMapper
```

Eccezione: `jackson-annotations` resta in `com.fasterxml.jackson.core`.

Classi rinominate:

| 2.7 / 3.x | 4.0 |
|---|---|
| `@JsonComponent` | `@JacksonComponent` |
| `@JsonMixin` | `@JacksonMixin` |
| `JsonObjectSerializer` | `ObjectValueSerializer` |
| `JsonValueDeserializer` | `ObjectValueDeserializer` |
| `Jackson2ObjectMapperBuilderCustomizer` | `JsonMapperBuilderCustomizer` |

Proprietà rinominate:

| 3.x | 4.0 |
|---|---|
| `spring.jackson.parser.*` | `spring.jackson.json.read.*` |
| `spring.jackson.read.*` | `spring.jackson.json.read.*` |
| `spring.jackson.write.*` | `spring.jackson.json.write.*` |

**Se non puoi migrare subito a Jackson 3**, usa il modulo di compatibilità:

```xml
<dependency>
    <groupId>org.springframework.boot</groupId>
    <artifactId>spring-boot-jackson2</artifactId>
</dependency>
```

Le proprietà Jackson 2 sono disponibili sotto `spring.jackson2.*`.

Per allineare il comportamento di Jackson 3 a Jackson 2:

```properties
spring.jackson.use-jackson2-defaults=true
```

### 15. Hibernate 6.x → 7.x

| 3.x | 4.0 |
|---|---|
| `hibernate-jpamodelgen` | `hibernate-processor` |
| `hibernate-proxool` | **rimosso** |
| `hibernate-vibur` | **rimosso** |

### 16. Proprietà rinominate (3.x → 4.0)

| 3.x | 4.0 |
|---|---|
| `spring.session.redis.*` | `spring.session.data.redis.*` |
| `spring.session.mongodb.*` | `spring.session.data.mongodb.*` |
| `spring.data.mongodb.*` (connessione) | `spring.mongodb.*` |
| `management.health.mongo.enabled` | `management.health.mongodb.enabled` |
| `management.metrics.mongo.command.enabled` | `management.metrics.mongodb.command.enabled` |
| `management.metrics.mongo.connectionpool.enabled` | `management.metrics.mongodb.connectionpool.enabled` |
| `spring.dao.exceptiontranslation.enabled` | `spring.persistence.exceptiontranslation.enabled` |

### 17. Funzionalità rimosse in 4.0

| Rimosso | Alternativa |
|---|---|
| **Undertow** | Non compatibile con Servlet 6.1. Usa Tomcat o Jetty |
| **Spring Session Hazelcast** | Usa il progetto Hazelcast direttamente |
| **Spring Session MongoDB** | Usa il progetto MongoDB direttamente |
| **Pulsar Reactive** | Rimosso (Spring Pulsar non supporta più reactive) |
| **Spock** | Non supporta Groovy 5 |
| **Executable uber jar launch scripts** | Usa `java -jar` o Gradle application plugin |
| **Classic uber-jar loader** | Rimuovi `<loaderImplementation>CLASSIC</loaderImplementation>` |
| **`HttpMessageConverters`** | Deprecato. Usa `ClientHttpMessageConvertersCustomizer` / `ServerHttpMessageConvertersCustomizer` |
| **LiveReload di DevTools** | Disabilitato di default. Setta `spring.devtools.livereload.enabled=true` |
| **Spring Retry** (dependency management) | Usa `org.springframework.core.retry` di Spring Framework 7 |
| **Spring Authorization Server** (proprietà versione) | Integrato in Spring Security 7. Usa `spring-security.version` |

### 18. Novità in 4.0 (non esistenti in 2.7)

| Feature | Dettaglio |
|---|---|
| **Spring gRPC** | Supporto first-class: server, client, security, health, testing |
| **AMQP 1.0 generico** | Starters separati: `spring-boot-starter-rabbitmq` e `spring-boot-starter-amqp` |
| **Native Image** | Production-ready con GraalVM |
| **Virtual Threads** | Pienamente supportati e migliorati |
| **Micrometer Observation API** | Integrata per metrics + tracing |
| **Spring Framework retry** | `org.springframework.core.retry` al posto di Spring Retry |
| **Spring Authorization Server** | Integrato in Spring Security 7 |
| **Jackson 3** | Default, con modulo compatibilità Jackson 2 |
| **Probes di liveness/readiness** | Abilitati di default |
| **Logback charset** | Default UTF-8 |
| **Elasticsearch Rest5Client** | Sostituisce il deprecato RestClient |
| **MongoDB UUID/BigDecimal** | Richiedono configurazione esplicita |
| **Modularizzazione completa** | ~60 moduli con package dedicati |

---

## Checklist Rapida

| # | Azione | Fase |
|---|---|---|
| 1 | Aggiorna all'ultima 2.7.x | Pre |
| 2 | Risolvi deprecation 2.7 | Pre |
| 3 | Migra Security a 5.8 pattern | Pre |
| 4 | Java 17+ | 2.7 → 3.x |
| 5 | `javax.*` → `jakarta.*` | 2.7 → 3.x |
| 6 | `spring.factories` → imports file | 2.7 → 3.x |
| 7 | Proprietà rinominate 3.x | 2.7 → 3.x |
| 8 | Hibernate 5 → 6 | 2.7 → 3.x |
| 9 | Spring Security 5 → 6 | 2.7 → 3.x |
| 10 | Testa su 3.5 | 2.7 → 3.x |
| 11 | Usa `spring-boot-starter-classic` | 3.x → 4.0 |
| 12 | Modularizza imports e dependencies | 3.x → 4.0 |
| 13 | Jackson 2 → 3 (o modulo compatibilità) | 3.x → 4.0 |
| 14 | Rimuovi Undertow se lo usavi | 3.x → 4.0 |
| 15 | Proprietà rinominate 4.0 | 3.x → 4.0 |
| 16 | Aggiorna entità MongoDB (UUID/BigDecimal) | 3.x → 4.0 |
| 17 | Rimuovi classic starters e finalizza | Fine |
| 18 | Rimuovi `spring-boot-properties-migrator` | Fine |

---

## Tempo stimato

| Dimensione progetto | Tempo stimato |
|---|---|
| Piccolo | 1-3 giorni |
| Medio | 1-2 settimane |
| Grande/Enterprise | 1-3 mesi |

Il collo di bottiglia principale è la fase `javax` → `jakarta` e la verifica dei test.

---

## Riferimenti

- [Spring Boot 3.0 Migration Guide](https://github.com/spring-projects/spring-boot/wiki/Spring-Boot-3.0-Migration-Guide)
- [Spring Boot 4.0 Migration Guide](https://github.com/spring-projects/spring-boot/wiki/Spring-Boot-4.0-Migration-Guide)
- [Spring Framework 6.x Upgrade Guide](https://github.com/spring-projects/spring-framework/wiki/Upgrading-to-Spring-Framework-6.x)
- [Spring Framework 7.0 Release Notes](https://github.com/spring-projects/spring-framework/wiki/Spring-Framework-7.0-Release-Notes)
- [OpenRewrite Javax → Jakarta](https://docs.openrewrite.org/recipes/java/migrate/jakarta/javaxmigrationtojakarta)
- [Spring Security 6.0 Migration](https://docs.spring.io/spring-security/reference/6.0/migration/index.html)
- [Spring Security 7.0 Migration](https://docs.spring.io/spring-security/reference/7.0/migration/)
- [Jackson 3.0 Release Notes](https://github.com/FasterXML/jackson/wiki/Jackson-Release-3.0)