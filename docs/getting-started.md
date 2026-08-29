# Getting started with SE

Create `hello.se`:

```se
name = ask "Name"
say "Hello " + name
```

Run it:

```bash
se run hello.se
```

Check it without running:

```bash
se check hello.se
```

Build a native executable:

```bash
se build hello.se
```

Source files should use `.se`. Legacy `.s` remains accepted temporarily for S 0.2 projects.
