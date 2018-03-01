var abvMap = {
    "Alabama": "01",
    "Alaska": "02",
    "Arizona": "04",
    "Arkansas": "05",
    "California": "06",
    "Colorado": "08",
    "Connecticut": "09",
    "Delaware": "10",
    "Florida": "12",
    "Georgia": "13",
    "Hawaii": "15",
    "Idaho": "16",
    "Illinois": "17",
    "Indiana": "18",
    "Iowa": "19",
    "Kansas": "20",
    "Kentucky": "21",
    "Louisiana": "22",
    "Maine": "23",
    "Maryland": "24",
    "Massachusetts": "25",
    "Michigan": "26",
    "Minnesota": "27",
    "Mississippi": "28",
    "Missouri": "29",
    "Montana": "30",
    "Nebraska": "31",
    "Nevada": "32",
    "New Hampshire": "33",
    "New Jersey": "34",
    "New Mexico": "35",
    "New York": "36",
    "North Carolina": "37",
    "North Dakota": "38",
    "Ohio": "39",
    "Oklahoma": "40",
    "Oregon": "41",
    "Pennsylvania": "42",
    "Rhode Island": "44",
    "South Carolina": "45",
    "South Dakota": "46",
    "Tennessee": "47",
    "Texas": "48",
    "Utah": "49",
    "Vermont": "50",
    "Virginia": "51",
    "Washington": "53",
    "West Virginia": "54",
    "Wisconsin": "55",
    "Wyoming": "56",
}
/*
fix hitting start button after
get a reset button
*/
document.getElementById("input").disabled = true;
$("#start").on("click", function() {
    document.getElementById("input").disabled = false;
    var input = document.getElementsByTagName("input");
    var states_found=[];
    input[0].value="";
    var time = document.getElementById("timer");
    var refreshId=setInterval(function(){
      time.textContent=(parseInt(time.textContent)-1).toString();
      $("#input").on("keyup", function() {
        var input = $("#input").val();
        var upperCaseStates=[];
        for(let key in abvMap) {
            upperCaseStates.push(key.toUpperCase());
        }

        if(upperCaseStates.indexOf(input.toUpperCase())!=-1&&states_found.indexOf(input.toUpperCase())==-1){
          for(let key in abvMap) {
            if(key.toUpperCase()==input.toUpperCase()){
              $("#container").append("<p class=\"astate\">"+key+"</p>");
            }
          }
          states_found.push(input.toUpperCase());
          $("#input").val("");
        }
        if(states_found.length==50){
          clearInterval(refreshId);
          $("#input").off("keyup");
          document.getElementById("loser").textContent="you win!";
          document.getElementById("input").disabled = true;
          document.getElementById("correctness").textContent="score:"+states_found.length.toString()+"/50";
        }
      });
      if(time.textContent=="0"){
        clearInterval(refreshId);
        $("#input").off("keyup");
        $("#input").val("");
        document.getElementById("input").disabled = true;
        document.getElementById("loser").textContent="you lose";
        document.getElementById("correctness").textContent=states_found.length.toString()+"/50";
        $("#container").append("<h2>What you missed</h2>");
        for(let key in abvMap) {
            if(states_found.indexOf(key.toUpperCase())==-1){
              $("#container").append("<p class=\"astate\">"+key+"</p>");
            }
        }
      }

    }, 1000);

});

$(document).on("mouseenter", ".astate", function(event) {
  $.get(" https://api.census.gov/data/2013/language?get=EST,LANLABEL,NAME&for=state:"+ abvMap[event.target.textContent]+"&LAN=625", function(data) {
      document.getElementById("spanish").textContent+=numberWithCommas(parseInt(data[1][0]));
  });
});
$(document).on("mouseleave", ".astate", function() {
    document.getElementById("spanish").textContent="Number of Spanish Speakers:";
});

function numberWithCommas(x){
  return x.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
}
