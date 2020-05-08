
    var mymap = L.map('mapid').setView([-22.94, -43.18], 13);

   setTimeout( function(){ mymap.invalidateSize(true); }, 100);

    L.tileLayer('http://{s}.tile.osm.org/{z}/{x}/{y}.png', { attribution: '&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors' }).addTo(mymap);


console.log(typeof(data));
var layerGroup = L.layerGroup().addTo(mymap);

function clicked(){
  layerGroup.clearLayers();
  var input_value = document.getElementById('data').value;
  for(i=0; i < busData.length; i++){



    LINHA = busData[i][0] 
    LATITUDE = busData[i][1] 
    LONGITUDE = busData[i][2] 
    DATA = busData[i][3].split(' ')[0];
    HORA = busData[i][3].split(' ')[1];
    VELOCIDADE = busData[i][4]

    //console.log("Linha:",LINHA,"Latitude:",LATITUDE,"Longitude",LONGITUDE,"Data:",DATA,"Hora:",HORA,"Velocidade:",VELOCIDADE)

    var LINHA = parseInt(LINHA);
    if (LINHA === parseInt(input_value)){
        let lat  = parseFloat(LATITUDE);
        let lon  = parseFloat(LONGITUDE);
        //console.log(DATA,HORA,VELOCIDADE);
        dataVel = `<h5>Linha: ${LINHA}</h5><h5>Data: ${DATA}</h5><h5>Hora: ${HORA}</h5><h5>Velocidade: ${VELOCIDADE}</h5>`
        L.marker([lat,lon]).bindPopup(dataVel).addTo(layerGroup);
    }
  };
}

function centerMe(){
  mymap.setView([-22.94, -43.18], 13);
};

document.getElementById('btn').addEventListener('click', clicked);;
document.getElementById('homeBtn').addEventListener('click', centerMe);;


