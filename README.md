# STC1000_arduino_upgrade -> STC1000a

Resumo: Projeto onde um controlador STC1000 (adaptado - upgraded by Arduino: **STC1000a**) realiza o disparo de um **controlador de motor basculante** para realizar abertura e fechamento de uma janela a partir da temperatura lida no sensor de temperatura. O **controlador de motor basculante** possui uma entrada para um contato aberto. A cada fechamento nesse contato ele realiza o acionamento de abertura/fechamento da janela, ficando para o **STC1000a** a função de gestão deste fechamento de contato a partir de suas configurações, leitura de temperatura e estado atual da janela.

<img width="623" height="328" alt="Image" src="https://github.com/user-attachments/assets/b485f5e4-152e-4826-b847-c078897c4aa1" />


| Menu | Descrição           | Teclas                       | Comentários                                         |   |
|------|---------------------|------------------------------|-----------------------------------------------------|---|
| Ab   | Temp. de abertura   | Cima: Aumenta Baixo: Diminui | -                                                   |   |
| Fe   | Temp. de fechamento | Cima: Aumenta Baixo: Diminui | -                                                   |   |
| Ma   | Manual              | Cima:Abre Baixo:Fecha        | Abre/fecha de acordo com estado do sensor magnético |   |
| Au   | Automático          | -                            | Caso sensor magnético esteja aberto                 |   |

Nota: Esse projeto foi concebido desta forma a partir de diversos desafios técnicos que foram encontrados pelo caminho. Por isso, foi desenvolvido o STC1000e, baseado em ESP32 com melhorias improtantes.