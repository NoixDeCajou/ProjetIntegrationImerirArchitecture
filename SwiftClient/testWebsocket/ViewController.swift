import UIKit
import Starscream
import SwiftyJSON
import Alamofire


class ViewController : UIViewController, WebSocketDelegate {
    
    var socket : WebSocket!
    
    var portWebSocket : Int = 0
    var id : Int = 0
    var map : String = ""
    var vertices = [[]]
    var edges = [[]]
    var cab : String = ""
    
    var TrucTest : String = "testCplF"

    override func viewDidLoad() {
        super.viewDidLoad()
        
        areaMap.setViewControler( self )

        print("in viewDidload")
        
        //let temp="{\"error\": false,\"portWebSocket\": 8000,\"id\": 2,\"map\": {\"areas\": [{\"map\": 5,\"mip\": 8}]}}"
        
        /*let temp="{\"error\" : false,\"portWebSocket\" : 8000,\"id\" : 2,\"map\" : {\"areas\" : [{\"map\" : {\"bridges\" : [{\"to\" : {\"area\" : \"Quartier Sud\",\"vertex\" : \"h\"},\"weight\" : 2,\"from\" : \"b\"}],\"vertices\" : [{\"y\" : 0.5,\"x\" : 0.5,\"name\" : \"m\"},{\"y\" : 1,\"x\" : 0.5,\"name\" : \"b\"}],\"weight\" : {\"h\" : 1,\"w\" : 1},\"streets\" : [{\"path\" : [\"m\",\"b\"],\"name\" : \"mb\",\"oneway\" : false}]},\"name\" : \"Quartier Nord\"},{\"map\" : {\"bridges\" : [{\"to\" : {\"area\" : \"Quartier Nord\",\"vertex\" : \"b\"},\"weight\" : 2,\"from\" : \"h\"}],\"vertices\" : [{\"y\" : 1,\"x\" : 1,\"name\" : \"a\"},{\"y\" : 1,\"x\" : 0,\"name\" : \"m\"},{\"y\" : 0,\"x\" : 0.5,\"name\" : \"h\"}],\"weight\" : {\"h\" : 1,\"w\" : 1},\"streets\" : [{\"path\" : [\"a\",\"h\"],\"name\" : \"ah\",\"oneway\" : false},{\"path\" : [\"m\",\"h\"],\"name\" : \"mh\",\"oneway\" : false}]},\"name\" : \"Quartier Sud\"}]}}"
        
        if let data = temp.dataUsingEncoding(NSUTF8StringEncoding)
        {
            let json = JSON(data: data)
            
            print(json)
            
            self.portWebSocket=json["portWebSocket"].intValue
            self.id=json["id"].intValue-1
            
            print("--------------------")
            
            //let dsfgds=json["map"]["areas"]["mip"].intValue
            
            var cpt=0
            for item in json["map"]["areas"].arrayValue {
                if(cpt==self.id)
                {
                    self.map=item["name"].description
                    print (self.map)
                }
                cpt++
            }
            
            print("--------------------")
            
            for item in json["map"]["areas"].arrayValue
            {
                if(item["name"].description==self.map)
                {
                    for vertex in item["map"]["vertices"].arrayValue
                    {
                        self.vertices.append([vertex["name"].description,vertex["x"].intValue,vertex["y"].intValue])
                    }
                }
            }
            
            print(self.vertices)
            
            print("--------------------")
            
            for item in json["map"]["areas"].arrayValue
            {
                if(item["name"].description==self.map)
                {
                    for vertex in item["map"]["streets"].arrayValue
                    {
                        self.edges.append([vertex["path"][0].description,vertex["path"][1].description])
                    }
                }
            }
            
            print(self.edges)
        }*/
        
        //Alamofire.request(.GET, "http://172.30.1.120:5000/monitor")
        Alamofire.request(.GET, "http://192.168.43.186:5000/monitor")
            .responseString { response in
                //print("Success: \(response.result.isSuccess)")
                //print("Response String: \(response.result.value)")

                if let data = response.result.value?.dataUsingEncoding(NSUTF8StringEncoding)
                {
                    let json = JSON(data: data)
                    
                    print(json)
                    
                    self.portWebSocket=json["portWebSocket"].intValue
                    self.id=json["id"].intValue
                    
                    print("--------------------")
                    
                    //let dsfgds=json["map"]["areas"]["mip"].intValue
                    
                    var cpt=0
                    for item in json["map"]["areas"].arrayValue {
                        if(cpt==self.id-1)
                        {
                            self.map=item["name"].description
                            print (self.map)
                        }
                        cpt++
                    }
                    
                    print("--------------------")
                    
                    self.vertices.removeAll()
                    for item in json["map"]["areas"].arrayValue
                    {
                        if(item["name"].description==self.map)
                        {
                            for vertex in item["map"]["vertices"].arrayValue
                            {
                                self.vertices.append([vertex["name"].description,vertex["x"].doubleValue,vertex["y"].doubleValue])
                            }
                        }
                    }
                    
                    print(self.vertices)
                    
                    print("--------------------")
                    
                    self.edges.removeAll()
                    for item in json["map"]["areas"].arrayValue
                    {
                        if(item["name"].description==self.map)
                        {
                            for vertex in item["map"]["streets"].arrayValue
                            {
                                self.edges.append([vertex["path"][0].description,vertex["path"][1].description])
                            }
                        }
                    }
                    
                    print(self.edges)

                }
                
                print("--------------------")
                self.areaMap.id = self.id
                self.areaMap.vertices = self.vertices
                self.areaMap.edges = self.edges
                self.areaMap.cab = self.cab
                
                self.areaMap.setNeedsDisplay()
                
                //self.socket = WebSocket(url: NSURL(string: "ws://localhost:8080/")!, protocols: ["chat", "superchat"])
                //self.socket = WebSocket(url: NSURL(string: "ws://172.30.1.120:"+self.portWebSocket.description+"/")!, protocols: ["chat", "superchat"])
                self.socket = WebSocket(url: NSURL(string: "ws://192.168.43.186:"+self.portWebSocket.description+"/")!, protocols: ["chat", "superchat"])
                
                self.socket.delegate = self
                self.socket.connect()
        }
    }

    // MARK: Websocket Delegate Methods.

    func websocketDidConnect(ws: WebSocket) {
        
        print("websocket is connected")
        socket.writeString("ploppipo")
        print("sending: ploppipo")
    }

    func websocketDidDisconnect(ws: WebSocket, error: NSError?) {
        if let e = error {
            print("websocket is disconnected: \(e.localizedDescription)")
        } else {
            print("websocket disconnected")
        }
    }

    func websocketDidReceiveMessage(ws: WebSocket, text: String) {
        print("Received text: \(text)")

        if let data = text.dataUsingEncoding(NSUTF8StringEncoding)
        {
            let json = JSON(data: data)
            
            print(json)
            
            print("--------------------")
            print("self.map: ")
            print(self.map)
            if(self.map==json["cabInfo"]["loc_now"]["area"].description)
            {
                print (0.description)
                self.cab=json["cabInfo"]["loc_now"]["location"].description
            }
            else
            {
                print (1.description)
                self.cab=""
            }
            print ("taxi :")
            print(self.cab)
            self.areaMap.cab=self.cab
            
            areaMap.setNeedsDisplay()
            print("--------------------")
        }

    }

    func websocketDidReceiveData(ws: WebSocket, data: NSData) {
        print("Received data: \(data.length)")
    }

    // MARK: Write Text Action

    @IBAction func writeText(sender: UIBarButtonItem) {
        socket.writeString("hello there!")
    }

    // MARK: Disconnect Action

    @IBAction func disconnect(sender: UIBarButtonItem) {
        if socket.isConnected {
            sender.title = "Connect"
            socket.disconnect()
        } else {
            sender.title = "Disconnect"
            socket.connect()
        }
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    
    // A chaque clic de la souris sur la surface aréa
    @IBAction func tap(sender: UITapGestureRecognizer)
    {
        
        // On affiche les cordonnes de notre tableau de point fait par la souris
        areaMap.points.append(
            sender.locationInView(areaMap)
        )
        
        // Rafraichissement de la vue
        areaMap.setNeedsDisplay()
    }
    
    @IBOutlet weak var areaMap: AreaMap!
    
    func sendRequestCab(nomPoint : String)
    {
        let msg = "{\"id\": "+self.id.description+" , \"cabRequest\": {\"area\": \""+self.map+"\",\"location\": {\"area\": \""+self.map+"\",\"locationType\": \"vertex\", \"location\": \""+nomPoint+"\"}}}"
        
        print("msg : "+msg)
        
        socket.writeString(msg)


    }
    
    
}

/*import UIKit

class ViewController: UIViewController {


override func viewDidLoad() {
super.viewDidLoad()
}

override func didReceiveMemoryWarning() {
super.didReceiveMemoryWarning()
// Dispose of any resources that can be recreated.
}


// A chaque clic de la souris sur la surface aréa
@IBAction func tap(sender: UITapGestureRecognizer) {

// On affiche les cordonnes de notre tableau de point fait par la souris
areaMap.points.append(
sender.locationInView(areaMap)
)

// Rafraichissement de la vue
areaMap.setNeedsDisplay()
}

@IBOutlet weak var areaMap: AreaMap!
}/*

/*let temp="{\"error\" : false,\"portWebSocket\" : 8000,\"id\" : 2,\"map\" : {\"areas\" : [{\"map\" : {\"bridges\" : [{\"to\" : {\"area\" : \"Quartier Sud\",\"vertex\" : \"h\"},\"weight\" : 2,\"from\" : \"b\"}],\"vertices\" : [{\"y\" : 0.5,\"x\" : 0.5,\"name\" : \"m\"},{\"y\" : 1,\"x\" : 0.5,\"name\" : \"b\"}],\"weight\" : {\"h\" : 1,\"w\" : 1},\"streets\" : [{\"path\" : [\"m\",\"b\"],\"name\" : \"mb\",\"oneway\" : false}]},\"name\" : \"Quartier Nord\"},{\"map\" : {\"bridges\" : [{\"to\" : {\"area\" : \"Quartier Nord\",\"vertex\" : \"b\"},\"weight\" : 2,\"from\" : \"h\"}],\"vertices\" : [{\"y\" : 1,\"x\" : 1,\"name\" : \"a\"},{\"y\" : 1,\"x\" : 0,\"name\" : \"m\"},{\"y\" : 0,\"x\" : 0.5,\"name\" : \"h\"}],\"weight\" : {\"h\" : 1,\"w\" : 1},\"streets\" : [{\"path\" : [\"a\",\"h\"],\"name\" : \"ah\",\"oneway\" : false},{\"path\" : [\"m\",\"h\"],\"name\" : \"mh\",\"oneway\" : false}]},\"name\" : \"Quartier Sud\"}]}}"*/

/*"{\"cabInfo\":{\"loc_now\": {\"locationType\": \"vertex\", \"location\": \"m\", \"area\": \"Quartier Nord\"}, \"destination\": {\"location\": {\"locationType\": \"vertex\", \"location\": \"h\", \"area\": \"Quartier Sud\"}, \"area\": \"Quartier Sud\"}, \"loc_prior\": {\"locationType\": \"vertex\", \"location\": \"b\", \"area\": \"Quartier Nord\"}, \"odometer\": 800}, \"nbCabRequest\":1, \"cabRequests\":[{\"location\": {\"locationType\": \"vertex\", \"location\": \"a\", \"area\": \"Quartier Sud\"}, \"idCabRequest\": 1, \"area\": \"Quartier Sud\"}]}"*/*/*/