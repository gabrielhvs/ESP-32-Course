# Protocolo da caracteristica de comando dos LEDs

## Exemplos de comando:

`0x0001FF0000` - LED 1 vermelho
`0x0006FF00FF` - LED 6 roxo
`0x01FF`       - Altera brilho da matriz para o brilho maximo
`0x02`         - Apaga todos os LEDs

## Descricao dos bytes:

[0] = Comando (0 = alterar LED, 1 = alterar brilho, 2 = limpar LEDs, 3 = piscar LEDs, 4 = alterar todos os LEDs)

### Comando 0 - Alterar LED

[1] = Numero do LED (0 a 0x0F)
[2] = Vermelho (0 a 0xFF)
[3] = Verde (0 a 0xFF)
[4] = Azul (0 a 0xFF) 

### Comando 1 - Alterar brilho

[1] = Intensidade (0 a 0xFF)

### Comando 2 - Limpar LEDs

(sem parametros)

### Comando 3 - Piscar LEDs 

(sem parametros - alterna entre on e off)

### Comando 4 - Alterar todos os LEDs

[1] = Vermelho (0 a 0xFF)
[2] = Verde (0 a 0xFF)
[3] = Azul (0 a 0xFF) 