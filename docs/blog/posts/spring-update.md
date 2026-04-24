---
title: "Spring Boot 2.7 → 4.0: Cambiamenti rilevanti per lo sviluppo"
date: 2026-04-24
description: "Evoluzione di Spring Boot dalla versione 2.7 alla 4.0, con confronto su Quarkus"
---

# Spring Boot 2.7 → 4.0: Cambiamenti rilevanti per lo sviluppo

## Spring Boot 3.0 *(LTS, Nov 2022)*

- **Java 17+ obbligatorio** — Fine supporto Java 8/11
- **Spring Framework 6** — Basato su Spring Framework 6
- **javax → jakarta** — Migration completa a Jakarta EE 9+ (`javax.servlet` → `jakarta.servlet`, ecc.)
- **GraalVM Native Image** — Supporto nativo per compilazione AOT
- **Micrometer Observation API** — Unified API per metriche e trace
- **Spring Security 6** — Configurazione security chain riscritta (metodo `securityFilterChain` al posto di `WebSecurityConfigurerAdapter`, rimosso)
- **@ConstructorBinding semplificato** — Non serve più su costruttori con singolo parametro
- **Elasticsearch Java Client** — Nuovo client auto-configurato
- **Observability** — Auto-config per Micrometer Tracing (Brave, OpenTelemetry, Zipkin, Wavefront)

## Spring Boot 3.1 *(Maggio 2023)*

- **Testcontainers** — Starter `spring-boot-testcontainers` e `@ServiceConnection`
- **SSL Bundle** — Auto-configurazione per gestione certificati SSL
- **Spring Authorization Server** — Auto-configurazione inclusa

## Spring Boot 3.2 *(Nov 2023)*

- **Virtual Threads** — `spring.threads.virtual.enabled=true` per abilitare (Java 21+)
- **RestClient** — Nuova API REST client fluente, stile WebClient ma blocking
- **JdbcClient** — Alternativa leggera a JdbcTemplate per query semplici
- **Spring for Apache Pulsar** — Nuova auto-config e starter
- **Logging Correlation IDs** — Trace ID e span ID inclusi automaticamente nei log
- **SSL Bundle Reloading** — Ricaricamento automatico certificati
- **Project CRaC** — Supporto iniziale per checkpoint/restore JVM
- **Observability in test** — `@AutoConfigureObservability` per test

## Spring Boot 3.3 *(Maggio 2024)*

- **CDS (Class Data Sharing)** — Supporto per startup più veloce con CDS
- **@MockitoBean/@MockitoSpyBean** — Introdotte come alternativa a `@MockBean`/`@SpyBean`
- **Service Connections** — Esteso supporto per più database/message broker
- **Docker Compose** — Miglioramenti nel service discovery

## Spring Boot 3.4 *(Nov 2024)*

- **Structured Logging** — Support built-in per ECS, Gelf, Logstash (`logging.structured.format.file=ecs`)
- **@Fallback beans** — `@ConditionalOnSingleCandidate` supporta `@Fallback`
- **Graceful Shutdown di default** — `server.shutdown=graceful` è ora il default
- **@MockBean/@SpyBean deprecate** — Usare `@MockitoBean`/`@MockitoSpyBean`
- **Actuator Endpoint Access** — Modello fine-grained (`none`, `read-only`, `unrestricted`) sostituisce `enabled/disabled`
- **MockMvcTester** — AssertJ support per MockMvc
- **ClientHttpRequestFactoryBuilder** — Builder fluente per configurare client HTTP
- **Base64 resources** — `base64:` protocol resolver per property
- **Docker Compose multi-file** — Supporto per più file docker-compose
- **Virtual Threads** — OtlpMeterRegistry e Undertow ora usano virtual threads se abilitati

## Spring Boot 3.5 *(Maggio 2025)*

- **Java 24 support** — Testato con JDK 24
- **Spring Framework 6.2.7**
- Miglioramenti vari a observability, Docker Compose, Service Connections

## Spring Boot 4.0 *(2026)*

- **Spring Framework 7** — Basato su Spring Framework 7.0
- **Java 21+ obbligatorio** — Baseline alzata a Java 21
- **Jakarta EE 10+** — Servlet 6.1, Persistence 3.2, Validation 3.1, ecc.
- **HTTP Service Clients** — Auto-config per interfacce `@HttpExchange` (`@GetExchange`, `@PostExchange`)
- **API Versioning** — Auto-config per versioning API in MVC e WebFlux (`spring.mvc.apiversion.*`)
- **JmsClient API** — Nuova API JMS semplificata (alternativa a JmsTemplate)
- **Task Decoration** — Supporto multi `TaskDecorator` bean con `CompositeTaskDecorator`
- **OpenTelemetry Starter** — `spring-boot-starter-opentelemetry`
- **Kotlin Serialization** — `spring-boot-starter-kotlin-serialization`
- **RestTestClient** — Test client analogo a WebTestClient ma per MockMvc
- **Redis Static Master/Replica** — Auto-config per Lettuce
- **Jackson 3** — Jackson 2 deprecato, Jackson 3 supportato
- **Jackson 3 / Gson 2.13** — Dipendenze aggiornate
- **Gradle 9** — Supporto per Gradle 9
- **Configuration Properties metadata** — `@ConfigurationPropertiesSource` per tipi esterni
- **Milestones su Maven Central** — Non serve più il repo Spring per milestones
- **`logging.console.enabled`** — Nuova property per disabilitare console logging

## Breakdown delle migration principali (2.7 → 3.x → 4.0)

### Da 2.7 a 3.0 (il salto più grande)
1. **Java 17+** obbligatorio
2. **javax → jakarta** — Rename di tutti i package Jakarta EE
3. **Spring Security rewrite** — `WebSecurityConfigurerAdapter` rimosso, usare `SecurityFilterChain` bean
4. **Native Image** — AOT processing obbligatorio per GraalVM
5. **Deprecazioni rimosse** — Molte API 2.x rimosse

### Da 3.x a 4.0
1. **Java 21+** obbligatorio
2. **Spring Framework 7** — Nuova major version
3. **Jackson 2 deprecato** — Prevedere migrazione a Jackson 3
4. **`@MockBean`/`@SpyBean` rimosse** — Già deprecate in 3.4
5. **Property migration** — Diverse property rinominate

## Per un progetto Quarkus: il confronto

| Feature | Quarkus | Spring Boot |
|---------|---------|-------------|
| Startup rapido | Native (sub-second) | Native Image (più lento), CDS (JVM) |
| Virtual Threads | Supportato | Supportato (da 3.2) |
| Dev mode | Hot-reload nativo | DevTools, LiveReload |
| Config injection | `@ConfigProperty` | `@Value`, `@ConfigurationProperties` |
| REST | JAX-RS / RESTEasy Reactive | Spring MVC / WebFlux |
| DI | Arc (CDI) | Spring IoC |
| Observability | Micrometer, OTel | Micrometer, OTel |
| GraalVM | First-class | Supportato da 3.0 |