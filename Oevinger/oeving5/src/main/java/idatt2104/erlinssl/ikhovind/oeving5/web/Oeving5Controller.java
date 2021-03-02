package idatt2104.erlinssl.ikhovind.oeving5.web;


import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;
import org.thymeleaf.spring5.expression.Mvc;

@Controller
public class Oeving5Controller {

    @RequestMapping("/welcome")
    public String welcome() {
        return "index";
    }
}
