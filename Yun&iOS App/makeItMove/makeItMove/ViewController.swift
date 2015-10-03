//
//  ViewController.swift
//  makeItMove
//
//  Created by Okan Agca on 03/10/15.
//  Copyright © 2015 ystkyrgn. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
    @IBOutlet var webView: UIWebView!

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        //let url = NSBundle.mainBundle().URLForResource("blockly", withExtension:"html")
        let url = NSBundle.mainBundle().URLForResource("rightLeft", withExtension:"html")
        let request = NSURLRequest(URL: url!)
        webView.loadRequest(request)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

