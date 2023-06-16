 <script>
    // Conexão com o broker MQTT
    var broker = 'broker.example.com';
    var port = 1883;
    var clientId = 'client_id';
    var topic = 'seu/topico';

    // Criando um cliente MQTT
    var client = new Paho.MQTT.Client(broker, port, clientId);

    // Definindo as funções de retorno de chamada
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;

    // Conectando-se ao broker MQTT
    client.connect({ onSuccess: onConnect });

    // Quando a conexão é estabelecida
    function onConnect() {
      console.log('Conectado ao broker MQTT');

      // Subscrevendo a um tópico
      client.subscribe(topic);
    }

    // Quando ocorre uma perda de conexão
    function onConnectionLost(responseObject) {
      if (responseObject.errorCode !== 0) {
        console.log('Conexão perdida: ' + responseObject.errorMessage);
      }
    }

    // Quando uma mensagem é recebida
    function onMessageArrived(message) {
      console.log('Mensagem recebida: ' + message.payloadString);
      // Exibindo a mensagem no contêiner
      var messageContainer = document.getElementById('message-container');
      messageContainer.innerHTML = message.payloadString;
    }