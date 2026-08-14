# Build usando VS Code + CMake

Passos rápidos para configurar e compilar o projeto no VS Code (Windows, MSVC):

1. Instale o CMake e o Visual Studio Build Tools (ou Visual Studio) com o componente "Desktop development with C++".
2. Abra a pasta do projeto no VS Code.
3. Execute a task de build (Ctrl+Shift+B) ou use a paleta: `Tasks: Run Task` → `cmake: build`.

Comandos manuais (PowerShell):
```powershell
cmake -S . -B build -A x64
cmake --build build --config Debug
```

Observações:
- O repositório já inclui as bibliotecas SDL em uma pasta compartilhada (`engines/SDL2-*/lib/x64`). O CMake configurado liga essas `.lib` automaticamente quando usa MSVC/x64.
- Os DLLs necessários serão copiados para a pasta do executável pelo passo pós-build.
- Se preferir outra toolchain (MinGW, clang) ajuste `-A`/o `compilerPath` em `.vscode/c_cpp_properties.json`.
