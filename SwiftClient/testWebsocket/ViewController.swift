import UIKit
import Starscream
import SwiftyJSON
import Alamofire


class ViewController : UIViewController, WebSocketDelegate {
    
    var socket : WebSocket!

    override func viewDidLoad() {
        super.viewDidLoad()

        print("in viewDidload")
        
        Alamofire.request(.GET, "http://172.30.1.120:5000/monitor")
            .responseString { response in
                //print("Success: \(response.result.isSuccess)")
                print("Response String: \(response.result.value)")
                print("--------------------")
        }
        
        //self.socket = WebSocket(url: NSURL(string: "ws://localhost:8080/")!, protocols: ["chat", "superchat"])
        self.socket = WebSocket(url: NSURL(string: "ws://172.30.1.120:8000/")!, protocols: ["chat", "superchat"])
        
        socket.delegate = self
        socket.connect()
    }

    // MARK: Websocket Delegate Methods.

    func websocketDidConnect(ws: WebSocket) {
        
        print("websocket is connected")
        socket.writeString("text test")
        print("sending: text test")
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
}



