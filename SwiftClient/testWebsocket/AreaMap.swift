//
//  AreaMap.swift
//  testWebsocket
//
//  Created by lemacboukaerien on 21/10/2015.
//  Copyright © 2015 lemacboukaerien. All rights reserved.
//

import Foundation
import UIKit

class AreaMap: UIView
{
    weak var vc : ViewController? = nil
    
    let width = 500.0
    let height = 550.0
    
    var id : Int = 99
    var vertices = [[]]
    var edges = [[]]
    var cab : String = ""
    
    // Tableau de point
    var points = [CGPoint(x:50, y:50)]
    
    // Position premier point
    let pointSize = CGSize(width: 20, height: 20)
    
    let line = UIBezierPath()
    
    var dictPointX = [ String : Double ]()
    var dictPointY = [ String : Double ]()
    
    func setViewControler( _vc : ViewController){
        
        self.vc = _vc
        
    }
    
    /*
    fonction récupérant les coordonné du click souris et les utilises pour déclancher
    l'évenement d'envoie d'une requete au serveur
    */
    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?)
    {
        print("entered touchesBegan")
        
        var plusProche = ""
        var bestDist = -1.0
        
        print(touches.first!.locationInView(self).x)
        print(touches.first!.locationInView(self).y)
        
        for vertex in self.vertices
        {
            var cpt=0
            var x=0.0
            var y=0.0
            var namedone = false
            var xdone = false
            var ydone = false
            var name = ""
            
            for detail in vertex
            {
                if(cpt==0)
                {
                    
                    name = detail.description
                    namedone = true
                    
                }
                
                if(cpt==1)
                {
                    x=detail.doubleValue
                    xdone = true
                }
                    
                else if(cpt==2)
                {
                    y=detail.doubleValue
                    ydone = true
                }
                
                if( (namedone == true) && (xdone == true) && (ydone == true))
                {
                    print(name+" : "+x.description+" : "+y.description)
                    
                    print("x*self.width")
                    print(x*self.width)
                    
                    print("y*self.height")
                    print(y*self.height)
                    
                    let xTemp=(x*self.width - Double(touches.first!.locationInView(self).x.description)!)
                    let yTemp=(y*self.height - Double(touches.first!.locationInView(self).y.description)!)
                    
                    print("Double(touches.first!.locationInView(self).x.description)!")
                    print(Double(touches.first!.locationInView(self).x.description)!)
                    
                    print("Double(touches.first!.locationInView(self).y.description)!")
                    print(Double(touches.first!.locationInView(self).y.description)!)
                    
                    print("xTemp")
                    print(xTemp)
                    
                    print("yTemp")
                    print(yTemp)
                    
                    if((bestDist == (-1.0)) || (bestDist > (sqrt(xTemp*xTemp+yTemp*yTemp))))
                    {
                        print("bestdist before")
                        print(bestDist)
                        
                        bestDist=(sqrt(xTemp*xTemp+yTemp*yTemp))
                        
                        print("bestdist after")
                        print(bestDist)
                        plusProche=name
                    }
                }
                
                cpt++
            }
        }
        
        print("PlusProche : "+plusProche)
        
        self.vc?.sendRequestCab( plusProche)
    }

    /*
    fonction dessinant la carte et le Taxi(Cab) en respectant les instruction donné préalablement par le`
    serveur et stocker dans le programme
    */
    override func drawRect(rect: CGRect)
    {
        /*UIColor.blackColor().set()
        
        var point1 = CGPoint(x:(width-1), y:(1))
        line.moveToPoint(point1)
        var point2 = CGPoint(x:(width-1), y:(height-1))
        line.addLineToPoint(point2)
        line.stroke()
        
        point1 = CGPoint(x:(width-1), y:(height-1))
        line.moveToPoint(point1)
        point2 = CGPoint(x:(1), y:(height-1))
        line.addLineToPoint(point2)
        line.stroke()
        
        point1 = CGPoint(x:(1), y:(height-1))
        line.moveToPoint(point1)
        point2 = CGPoint(x:(1), y:(1))
        line.addLineToPoint(point2)
        line.stroke()*/
        
        print("Test wcpl :")
        print (self.vc!.TrucTest)
        
        print("AREAMAP :")
        
        //print(self.id)
        //print(self.vertices)
        //print(self.edges)
        //print(self.cab)
        
        UIColor.redColor().set()
        
        /*let point1 = CGPoint(x:50, y:50)
        ligne.moveToPoint(point1)
        let point2 = CGPoint(x:100, y:100)
        ligne.addLineToPoint(point2)
        ligne.stroke()*/
        
        print(self.vertices)
        for vertex in self.vertices
        {
            var cpt=0
            var x=0.0
            var y=0.0
            var name = ""
            var namedone = false
            var xdone = false
            var ydone = false
            
            for detail in vertex
            {
                if(cpt==0){
                    
                    name = detail.description
                    namedone = true
                    
                    print("name of vertex")
                    print(name)
                    
                }
                
                if(cpt==1)
                {
                    x=detail.doubleValue
                    xdone = true
                }
                
                else if(cpt==2)
                {
                    y=detail.doubleValue
                    ydone = true
                }
                
                if( (namedone == true) && (xdone == true) && (ydone == true) ){
                    
                    dictPointX[name] = x
                    dictPointY[name] = y

                    
                }
                
                cpt++
            }
            
            print (x.description+" : "+y.description)
            let point = CGPoint(x:x*width-10, y:y*height-10)
            let cercle = UIBezierPath(ovalInRect: CGRect(origin:point, size:pointSize))
            cercle.fill()
        }
        
        print("--------------------")
        
        print("dictPointX:")
        print(dictPointX)

        print("dictPointY:")
        print(dictPointY)

        
        print("--------------------")
        
        
        UIColor.grayColor().set()
        
        var cpt=0
        
        //print(self.edges)
        for arc in self.edges
        {
            print("arc: ")
            print(arc)
            
            var x1 = 0.0
            var x2 = 0.0
            var y1 = 0.0
            var y2 = 0.0
            
            cpt = 0
            
            var fromDone = false
            var toDone = false
            
            for point in arc{
                
                if(cpt == 0){
                    // from
                    print("point from")
                    print(point)
                    
                    x1 = dictPointX[point as! String]!
                    y1 = dictPointY[point as! String]!
                    
                    print("x1, y1")
                    print(x1)
                    print(y1)
                    
                    fromDone = true
                    
                }
                else if(cpt == 1) {
                    //to
                    print("point to")
                    print(point)
                    
                    x2 = dictPointX[point as! String]!
                    y2 = dictPointY[point as! String]!

                    print("x2, y2")
                    print(x2)
                    print(y2)
                    
                    toDone = true
                    
                }
                
                if( (fromDone == true) && (toDone == true) ){
                    
                    print("dessin d'un trait :")
                    print("x1:" + x1.description)
                    print("y1:" + y1.description)
                    print("x2:" + x2.description)
                    print("y2:" + y2.description)

                    
                    
                    let point1 = CGPoint(x:(x1*width), y:(y1*height))
                    line.moveToPoint(point1)
                    let point2 = CGPoint(x:(x2*width), y:(y2*height))
                    line.addLineToPoint(point2)
                    line.stroke()
                    
                }
                
                cpt++
            }
            
            print("--------------------")
                        print("self.cab")
            print (self.cab)
            
            let xCab = dictPointX[self.cab]
            let yCab = dictPointY[self.cab]
            
            print("xcab, ycab")
            print(xCab)
            print(yCab)
            
            if( (xCab != nil) && ( yCab != nil) )
            {
                UIColor.greenColor().set()
                let point = CGPoint(x:xCab!*width-10, y:yCab!*height-10)
                let cercle = UIBezierPath(ovalInRect: CGRect(origin:point, size:pointSize))
                cercle.fill()
            }
        }
        
        print("--------------------")
    }
    
}
