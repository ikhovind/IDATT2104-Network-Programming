package idatt2104.erlinssl.ikhovind.oeving5.web;


import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;
import org.thymeleaf.spring5.expression.Mvc;

import java.io.BufferedWriter;
import java.io.FileDescriptor;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URLDecoder;
import java.nio.charset.StandardCharsets;
import org.apache.commons.text.StringEscapeUtils;

@Controller
public class Oeving5Controller {

    @RequestMapping("/welcome")
    public String welcome() {
        return "index";
    }

    @PostMapping("/programs")
    public String enterCode(@RequestBody String input) throws IOException {
        String unEscapedHTML = URLDecoder.decode(input);
        BufferedWriter writer = new BufferedWriter(new FileWriter("/docker/out.cpp"));
        System.out.println(unEscapedHTML);
        writer.write(unEscapedHTML.substring(10));
        writer.close();
        return "index";
    }
}
