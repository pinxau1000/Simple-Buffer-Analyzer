# Transmissão de vídeo em tempo-real verificação de conformidade e análise de requisitos
- [Transmissão de vídeo em tempo-real verificação de conformidade e análise de requisitos](#transmissão-de-vídeo-em-tempo-real-verificação-de-conformidade-e-análise-de-requisitos)
  - [1. Objetivos](#1-objetivos)
  - [2. Especificações](#2-especificações)
    - [2.1 Verificação de conformidade](#21-verificação-de-conformidade)
    - [2.2. Débito binário médio](#22-débito-binário-médio)
    - [2.3. Débitos binários máximo e mínimo em VBR](#23-débitos-binários-máximo-e-mínimo-em-vbr)
    - [2.4. Transmissão VBR](#24-transmissão-vbr)


## 1. Objetivos
- Pretende-se desenvolver uma ferramenta de software (para ser executada em linha de comando em C/C++) que permita determinar características de streams de vídeo codificados, em termos de parâmetros relevantes relativos ao débito binário, para transmissão em tempo real. Os dados a analisar (input) consistem unicamente no número de bits/frame após codificação, sendo conhecida taxa temporal em frames/seg.
- Pretende-se verificar se um dado stream cumpre com requisitos diversos relativamente à transmissão em tempo real, com débito constante (CBR) ou variável (VBR), considerando o tamanho do buffer do descodificador.


## 2. Especificações
Neste trabalho pretende-se desenvolver uma ferramenta de software com as funcionalidades, entradas e saídas que se especificam a seguir.

A informação relativa ao stream de vídeo codificado consiste num trace de vídeo, no formato csv, onde cada linha contém 2 números separados por vírgulas. A primeira linha contém a taxa temporal do vídeo (em frames/seg), seguido de um zero. Cada uma das linhas seguintes contém aos dados relativos a uma imagem (frame): o primeiro número corresponde ao número de bytes com que a frame foi codificada e o segundo corresponde ao PSNR da luminância (Y). O programa a desenvolver deve executar as 4 funcionalidades seguintes:

### 2.1 Verificação de conformidade
Pretende-se verificar se a descodificação do stream em tempo real é possível, considerando um determinado débito de rede (CBR) e tamanho de buffer, ambos fornecidos como parâmetros de entrada. Se não for possível, a respetiva ocorrência deve ser caracterizada com informação de saída (e.g. Buffer underflow na frame no <x>)

### 2.2. Débito binário médio
Pretende-se obter uma saída numérica que corresponda ao débito binário médio do stream de entrada (em kbps).

### 2.3. Débitos binários máximo e mínimo em VBR
Pretende-se determinar os débitos binários máximo e mínimo (informação de saída) que devem ser suportados por uma rede com capacidade de transmissão com débitos variáveis (VBR), de modo a garantir que o atraso total do sistema de comunicação não é superior ao período correspondente a uma imagem (frame).

### 2.4. Transmissão VBR
Pretende-se que o software implemente um método automático para determinar as taxas de transmissão VBR (definidas como CBR dentro de cada intervalo de tempo TR, que é um parâmetro de entrada) que garantem o funcionamento de um descodificador em tempo real. Deve determinar também o tamanho mínimo de um buffer B para usar no codificador e descodificador que seja suficiente para armazenar as imagens de um segmento temporal TR. Depois deve simular o sistema e verificar se funciona em tempo real.
