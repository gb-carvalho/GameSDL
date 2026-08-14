
# GameSDL

## Sobre o Projeto

GameSDL é um jogo desenvolvido em C++ utilizando a biblioteca [SDL (Simple DirectMedia Layer)](https://www.libsdl.org/). O objetivo principal do projeto é explorar e aprimorar os fundamentos da linguagem C++ e o desenvolvimento de jogos com SDL, proporcionando uma excelente oportunidade de estudo e prática.

## Funcionalidades

-   Gráficos renderizados com SDL2
    
-   Suporte a imagens via SDL2_image
    
-   Sistema de áudio com SDL2_mixer
    
-   Renderização de textos com SDL2_ttf
    
-   Mecânica de jogo baseada em tempo de sobrevivência
    
-   Funções de salvar e carregar progresso (kill_count e elapsed_time)
    
-   Interface refinada com ícone personalizado na janela e no executável
    

## Requisitos

Para compilar e rodar o GameSDL, você precisará dos seguintes componentes:

-   [SDL2](https://github.com/libsdl-org/SDL/releases)
    
-   [SDL2_image](https://github.com/libsdl-org/SDL_image/releases)
    
-   [SDL2_mixer](https://github.com/libsdl-org/SDL_mixer/releases)
    
-   [SDL2_ttf](https://github.com/libsdl-org/SDL_ttf/releases)
    
-   Compilador compatível com C++ (como MSVC no Visual Studio)
    

## Como Compilar e Executar

### Windows (VS Code + CMake)

1.  Clone o repositório:
    
    ```
    git clone https://github.com/seu-usuario/GameSDL.git
    cd GameSDL
    ```
    
2.  Abra a pasta no VS Code.
    
3.  Execute a task de build do CMake ou rode manualmente:

    ```powershell
    cmake -S . -B build -A x64
    cmake --build build --config Debug
    ```

4.  O executável será gerado em `build/Debug/GameSDL.exe`.
    

## Estrutura do Projeto

```
GameSDL/
├── Assets/
│   ├── audio/
│   ├── fonts/
│   ├── images/
│   ├── legacy/
│   ├── inspiration/
│   └── license/
├── engines/
│   ├── SDL2-2.30.8/
│   ├── SDL2_image-2.8.2/
│   ├── SDL2_mixer-2.8.0/
│   └── SDL2_ttf-2.20.2/
├── include/
├── src/
├── build/
├── CMakeLists.txt
├── .vscode/
├── .gitignore
├── LICENSE
├── README.md
└── README_VSCODE_BUILD.md
```

## Licença

Este projeto está licenciado sob a [MIT License](LICENSE).

----------
