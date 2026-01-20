# Contributing to MPC

## Reporting Issues

- Check existing issues before creating duplicates
- Include minimal reproduction case
- Specify platform, compiler, and MPC version
- Provide relevant code snippets and error output

## Pull Request Process

1. Fork the repository to your GitHub account
2. Clone your fork locally:

```bash
   git clone https://github.com/yourusername/mpc.git
   cd mpc
```

3. Add upstream remote:

```bash
   git remote add upstream https://github.com/orangeduck/mpc.git
```

4. Create a branch for your changes:

```bash
   git checkout -b fix/descriptive-name
```

5. Make your changes
6. Test thoroughly on your platform
7. Commit with clear, imperative messages:

```bash
   git commit -m "Fix segfault in mpc_parse cleanup"
```

8. Rebase on latest upstream before pushing:

```bash
   git fetch upstream
   git rebase upstream/master
```

9. Push to your fork:

```bash
   git push origin fix/descriptive-name
```

10. Open pull request from your branch to `orangeduck/mpc:master`

## Branch Naming

- `fix/` - Bug fixes
- `feature/` - New functionality
- `docs/` - Documentation changes

## Code Standards

- Match existing code style
- Single responsibility per commit
- No unrelated changes in PRs
- Include test cases when applicable

## Testing

- Compile without warnings
- Run existing tests if present
- Verify your fix resolves the reported issue
- Test on multiple compilers if possible (gcc, clang, msvc)
