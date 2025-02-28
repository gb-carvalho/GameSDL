
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

### Windows (Visual Studio)

1.  Clone o repositório:
    
    ```
    git clone https://github.com/seu-usuario/GameSDL.git
    cd GameSDL
    ```
    
2.  Abra o arquivo `GameSDL.sln` no Visual Studio.
    
3.  Compile o projeto e execute.
    

## Estrutura do Projeto

```
GameSDL/
├── Assets/         # Recursos visuais e de áudio do jogo
├── include/        # Cabeçalhos (.h) utilizados no projeto
├── src/            # Código-fonte do jogo
├── GameSDL.sln     # Solução do Visual Studio
├── GameSDL.vcxproj # Arquivo de projeto do Visual Studio
├── resources.rc    # Recursos do Windows (como ícones)
├── .gitignore      # Arquivo para ignorar arquivos desnecessários no Git
├── README.md       # Documentação do projeto
```

## Licença

Este projeto está licenciado sob a MIT License.

----------
