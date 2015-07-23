    var image = new Image();
    var timeout;

    function imgNewName()
    {
      var val = Math.random()*(1000.99);
      var res = "jpg-image?d=" + val.toString();
      return res;
    }

    function onTimeout( tm )
    {
      clearTimeout( timeout );
      var name = imgNewName();
      image.src = name;
    }

    function loadHandler()
    {
      console.log( "load handler" );
      $( "#img" ).attr( "src", image.src );
      timeout = setTimeout( onTimeout, 200 );
    }
    
    function documentReady()
    {
      console.log( "document ready" );
      var name = imgNewName();
      image.onload = loadHandler;
      image.src = name;
      //setInterval( loadHandler, 1000 );
      timeout = setTimeout( onTimeout, 1000 );
    }
    
    console.log( "before setting handler" );
    $( document ).ready( documentReady );
    console.log( "after setting handler" );
