---
title: "Principi SOLID - Sintesi"
date: 2026-04-24
description: "Sintesi dei principi SOLID di Robert C. Martin (Uncle Bob)"
categories:
  - Java
---

# Principi SOLID - Sintesi

> Robert C. Martin (Uncle Bob)

---

## Introduzione

I principi SOLID sono 5 linee guida per la progettazione software orientata agli oggetti che mirano a rendere il codice più **manutenibile**, **flessibile** e **comprensibile**.

---

## 1. S - Single Responsibility Principle (SRP)

> *"Una classe dovrebbe avere un solo motivo per cambiare."*

### Concetto
Una classe o modulo dovrebbe essere responsabile di una sola parte della funzionalità del software, e questa responsabilità dovrebbe essere interamente incapsulata dalla classe.

### Problema che risolve
Classi "dio" (God Classes) che fanno troppo, diventando difficili da capire, testare e modificare.

### Esempio
```java
// ❌ BAD: La classe gestisce sia dati utente che persistenza e notifiche
class UserManager {
    void saveUser(User user) { ... }
    void sendEmail(User user, String message) { ... }
    void generateReport(User user) { ... }
}

// ✅ GOOD: Responsabilità separate
class UserRepository {
    void save(User user) { ... }
}

class EmailService {
    void send(User user, String message) { ... }
}

class ReportGenerator {
    void generateFor(User user) { ... }
}
```

---

## 2. O - Open/Closed Principle (OCP)

> *"Le entità software dovrebbero essere aperte all'estensione, ma chiuse alla modifica."*

### Concetto
Dovresti poter estendere il comportamento di una classe senza modificarne il codice sorgente.

### Problema che risolve
Modifiche al codice esistente che introducono bug in parti già funzionanti.

### Esempio
```java
// ❌ BAD: Ogni nuova forma richiede di modificare AreaCalculator
class AreaCalculator {
    double calculate(Object shape) {
        if (shape instanceof Rectangle) {
            Rectangle r = (Rectangle) shape;
            return r.width * r.height;
        } else if (shape instanceof Circle) {
            Circle c = (Circle) shape;
            return Math.PI * c.radius * c.radius;
        }
        return 0;
    }
}

// ✅ GOOD: Nuove forme si aggiungono senza modificare AreaCalculator
interface Shape {
    double area();
}

class Rectangle implements Shape {
    double width, height;
    public double area() { return width * height; }
}

class Circle implements Shape {
    double radius;
    public double area() { return Math.PI * radius * radius; }
}

class AreaCalculator {
    double calculate(Shape shape) {
        return shape.area();
    }
}
```

---

## 3. L - Liskov Substitution Principle (LSP)

> *"Gli oggetti di una classe derivata devono poter sostituire gli oggetti della classe base senza alterare il comportamento corretto del programma."*

### Concetto
Se `S` è un sottotipo di `T`, allora gli oggetti di tipo `T` possono essere sostituiti con oggetti di tipo `S` senza cambiare proprietà desiderabili del programma.

### Problema che risolve
Gerarchie di ereditarietà che violano il comportamento atteso, causando bug sottili.

### Esempio
```java
// ❌ BAD: Quadrato viola il contratto di Rettangolo
class Rectangle {
    void setWidth(double w) { this.width = w; }
    void setHeight(double h) { this.height = h; }
}

class Square extends Rectangle {
    // Viola LSP: se setto width, cambia anche height
    void setWidth(double w) { this.width = w; this.height = w; }
    void setHeight(double h) { this.width = h; this.height = h; }
}

// ✅ GOOD: Non forzare l'ereditarietà se viola il contratto
interface Shape {
    double area();
}

class Rectangle implements Shape { ... }
class Square implements Shape { ... }
```

---

## 4. I - Interface Segregation Principle (ISP)

> *"Nessun client dovrebbe essere costretto a dipendere da metodi che non usa."*

### Concetto
È meglio avere molte interfacce specifiche che una sola interfaccia generica "grassa".

### Problema che risolve
Classi che devono implementare metodi che non servono, creando dipendenze inutili.

### Esempio
```java
// ❌ BAD: Worker è forzato a implementare eat() anche se è un robot
interface Worker {
    void work();
    void eat();
}

class Human implements Worker {
    void work() { ... }
    void eat() { ... }
}

class Robot implements Worker {
    void work() { ... }
    void eat() { /* Non fa nulla! */ } // Violazione ISP
}

// ✅ GOOD: Interfacce separate
interface Workable {
    void work();
}

interface Feedable {
    void eat();
}

class Human implements Workable, Feedable {
    void work() { ... }
    void eat() { ... }
}

class Robot implements Workable {
    void work() { ... }
}
```

---

## 5. D - Dependency Inversion Principle (DIP)

> *"I moduli di alto livello non dovrebbero dipendere da quelli di basso livello. Entrambi dovrebbero dipendere da astrazioni."*

### Concetto
Dipendi da astrazioni (interfacce/abstract class), non da classi concrete.

### Problema che risolve
Accoppiamento stretto tra moduli che rende difficile testare e sostituire implementazioni.

### Esempio
```java
// ❌ BAD: Il servizio di alto livello dipende direttamente dal database
class MySQLDatabase {
    void save(String data) { ... }
}

class UserService {
    private MySQLDatabase db = new MySQLDatabase(); // Accoppiamento stretto
    
    void createUser(String data) {
        db.save(data);
    }
}

// ✅ GOOD: Dipendenza da astrazione (interfaccia)
interface Database {
    void save(String data);
}

class MySQLDatabase implements Database {
    void save(String data) { ... }
}

class MongoDatabase implements Database {
    void save(String data) { ... }
}

class UserService {
    private Database db;
    
    UserService(Database db) { // Dependency Injection
        this.db = db;
    }
    
    void createUser(String data) {
        db.save(data);
    }
}

// Uso
UserService service = new UserService(new MySQLDatabase());
// Oppure facilmente sostituibile:
UserService service = new UserService(new MongoDatabase());
```

---

## Riassunto Rapido

| Principio | Domanda Chiave |
|-----------|----------------|
| **S**RP | La classe fa una sola cosa? |
| **O**CP | Devo modificare il codice esistente per aggiungere funzionalità? |
| **L**SP | Posso sostituire la classe figlia con la classe padre senza problemi? |
| **I**SP | L'interfaccia è troppo "grasa"? |
| **D**IP | Dipendo da classi concrete o da astrazioni? |

---

## Benefici

- **Manutenibilità**: modifiche più sicure e localizzate
- **Testabilità**: più facile scrivere unit test con mock
- **Flessibilità**: più facile cambiare implementazioni
- **Riutilizzabilità**: componenti più modulari e indipendenti
- **Comprensibilità**: codebase più facile da capire

---

> *"I principi SOLID non sono regole rigide, ma linee guida che aiutano a prendere decisioni di progettazione migliori."* - Robert C. Martin