const floorNames = new Map();
floorNames.set('ELS01', new Map([
    [3, 'Street Level'],
    [2, 'Tunnels'],
    [1, 'Cell Block']]));
floorNames.set('ELS02', new Map([
    [1, 'Route Moyenne Corniche'],
    [2, 'Monaco Railway'],
    [3, 'Shoreline']]));
floorNames.set('EPP01', new Map([
    [1, 'Route Moyenne Corniche'],
    [2, 'Monaco Railway'],
    [3, 'Shoreline']]));
floorNames.set('EPP02', new Map([
    [3, 'Main Office'],
    [2, 'Second Floor'],
    [1, 'Street Level']]));
floorNames.set('P01', new Map([
    [1, 'Underground Caverns']]));
floorNames.set('P02', new Map([
    [1, 'Sun City']]));
floorNames.set('F01', new Map([
    [4, 'Helipad'],
    [3, 'Penthouse Floor'],
    [2, 'Apartment Floor'],
    [1, 'Lobby Floor']]));
floorNames.set('F02', new Map([
    [3, 'Overlook'],
    [2, 'Second Floor'],
    [1, 'Street Level']]));

function createSelects() {  
    let mapSelects = new Map();
    for(let [mapId, mapFloors] of floorNames) {
        let selectNode = document.createElement('select');
        selectNode.name = 'select-floor';
        selectNode.id = 'select-floor';
        for(let [number, name] of mapFloors) {
            let option = document.createElement('option');
            option.value = number;
            option.textContent = name;
            selectNode.appendChild(option);
        }
        mapSelects.set(mapId, selectNode);
    }
    return mapSelects;
}

let mapSelects = createSelects();

let selectLevel = document.getElementById('select-level');
let selectFloor = document.getElementById('select-floor');
let blueprint = document.getElementById('blueprint');
let map = "ELS01";
let floor = "1";

selectLevel.addEventListener('change', (event) => {
    map = event.target.value;
    floor = "1";
    blueprint.src = 'images/' + map + '-' + floor + '.jpg';
    //TODO: add alt name
    blueprint.alt = 'fill this in later';

    selectFloor.replaceWith(mapSelects.get(map));
    selectFloor = document.getElementById('select-floor');
    selectFloor.addEventListener('change', (event) => {
        floor = event.target.value;
        blueprint.src = 'images/' + map + '-' + floor + '.jpg';
        //TODO: add floor name in alt
        blueprint.alt = 'put floor name here';
    });
    

    selectFloor.value = floor;
});

selectFloor.addEventListener('change', (event) => {
    floor = event.target.value;
    blueprint.src = 'images/' + map + '-' + floor + '.jpg';
    //TODO: add floor name in alt
    blueprint.alt = 'put floor name here';
});


